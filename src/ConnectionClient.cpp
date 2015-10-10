// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "ConnectionClient.h"

#include "DebugPrinter.h"
#include "ResultController.h"
#include "ResultEntry.h"

const int ConnectionRetryIntervalMs = 5000;
const int ServerPingIntervalMs = 5000;

const my_bool my_Enable = 1;

static ColumnInfo::ColumnType toColumnType(enum_field_types type) {
	switch (type) {
	case MYSQL_TYPE_DECIMAL:
	case MYSQL_TYPE_FLOAT:
	case MYSQL_TYPE_DOUBLE:
		return ColumnInfo::FloatColumnType;

	case MYSQL_TYPE_TINY:
	case MYSQL_TYPE_SHORT:
	case MYSQL_TYPE_LONG:
	case MYSQL_TYPE_BIT:
	case MYSQL_TYPE_LONGLONG:
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_YEAR:
		return ColumnInfo::IntegerColumnType;
	}

	return ColumnInfo::StringColumnType;
}

// -----------------------------------------------------------------------------
// Client-thread methods.

void ConnectionClient::doConnect() {
	DebugPrinter::Print("[MySQL] Connecting to %s:%d using username %s...", connection_status_.information.hostname.c_str(),
		connection_status_.information.port, connection_status_.information.username.c_str());

	mysql_thread_init();
	mysql_options(&connection_, MYSQL_OPT_RECONNECT, &my_Enable);

	ConnectionMessages::ConnectionAttemptResult result;
	result.hostname = connection_status_.information.hostname;
	result.username = connection_status_.information.username;

	if (mysql_real_connect(&connection_, connection_status_.information.hostname.c_str(),
			connection_status_.information.username.c_str(), connection_status_.information.password.c_str(),
			connection_status_.information.database.c_str(), connection_status_.information.port, NULL, 0))
	{
		result.succeeded = true;
		result.error = "";
		result.error_number = 0;

		// Mark the connection as being connected.
		connection_status_.is_connected = true;
	}
	else
	{
		result.succeeded = false;
		result.error = std::string(mysql_error(&connection_));
		result.error_number = mysql_errno(&connection_);
	}

	connection_attempt_queue_.push(result);
}

void ConnectionClient::doClose() {
	DebugPrinter::Print("[MySQL] Closing the connection with %s...", connection_status_.information.hostname.c_str());
	mysql_close(&connection_);
}

void ConnectionClient::doPing() {
	mysql_ping(&connection_);
}

void ConnectionClient::doQuery(const std::string& query, const std::string& callback, unsigned int data_id, ExecutionType execution_type) {
	DebugPrinter::Print("[MySQL] Executing query: \"%s\".", query.c_str());
	if (mysql_real_query(&connection_, query.c_str(), query.size()) == 0) {
		MYSQL_RES* queryResult = mysql_store_result(&connection_);
		ResultEntry* result = 0;

		// In some cases we might not want any feedback at all.
		if (!callback.length() || execution_type == SilentExecution)
			return;

		if (queryResult != 0) {
			// This was a SELECT query that returned rows.
			result = new ResultEntry();
			int field_count = static_cast<int>(mysql_num_fields(queryResult));
			int row_count = static_cast<int>(mysql_num_rows(queryResult));

			if (row_count > 0) {
				MYSQL_FIELD* fields = mysql_fetch_fields(queryResult);
				for (int fieldId = 0; fieldId < field_count; ++fieldId)
					result->addColumn(fields[fieldId].name, toColumnType(fields[fieldId].type));

				while (MYSQL_ROW resultRow = mysql_fetch_row(queryResult)) {
					unsigned long* fieldLengths = mysql_fetch_lengths(queryResult);
					RowInfo* row = result->createRow();

					for (int fieldId = 0; fieldId < field_count; ++fieldId)
						row->pushField(resultRow[fieldId], fieldLengths[fieldId], toColumnType(fields[fieldId].type));
				}
			}

			mysql_free_result(queryResult);
		} else {
			// This was an INSERT, UPDATE or DELETE query that shouldn't return rows.
			if (mysql_field_count(&connection_) != 0) {
				DebugPrinter::Print("[MySQL] Error while fetching data: %s (%d)", mysql_error(&connection_), mysql_errno(&connection_));
				DebugPrinter::Print("    --> Query: \"%s\"", query.c_str());
				DebugPrinter::Print("    --> Callback: \"%s\"", callback.c_str());
			} else {
				// No data-retrieval errors occurred, continue with filling the result.
				result = new ResultEntry();
				result->affected_rows_ = static_cast<int>(mysql_affected_rows(&connection_));
				result->insert_id_ = static_cast<int>(mysql_insert_id(&connection_));
			}
		}

		// Only register this result row if we were able to get a result.
		if (result) {
			ConnectionMessages::SucceededQueryResult successResult;
			successResult.result_id = ResultController::instance()->push(result);
			successResult.callback = callback;
			successResult.data_id = data_id;

			succeeded_query_queue_.push(successResult);
			return;
		}
	}

	if (execution_type == SilentExecution)
		return;

	ConnectionMessages::FailedQueryResult errorResult;
	errorResult.error = std::string(mysql_error(&connection_));
	errorResult.error_number = mysql_errno(&connection_);
	errorResult.callback = callback;
	errorResult.query = query;

	failed_query_queue_.push(errorResult);
}

void ConnectionClient::run() {
	while (!shutdown_requested()) {
		if (connection_status_.has_connection_information == false) {
			if (connection_queue_.size() > 0) {
				// We received connection information for this connection client.
				connection_queue_.pop(connection_status_.information);
				connection_status_.has_connection_information = true;
				continue;
			}

			thread_sleep(25);
			continue;
		}

		if (connection_status_.is_connected == false) {
			if (connection_status_.last_attempt == 0 || timeSpan(connection_status_.last_attempt) > ConnectionRetryIntervalMs) {
				doConnect(); // attempt to connect to the server.
				connection_status_.last_attempt = time();
				continue;
			}

			thread_sleep(25);
			continue;
		}

		if (connection_status_.last_ping == 0 || timeSpan(connection_status_.last_ping) > ServerPingIntervalMs) {
			connection_status_.last_ping = time();
			doPing();
			continue;
		}
		
		if (query_queue_.size() > 0) {
			ConnectionMessages::QueryInformation information;
			query_queue_.pop(information);

			// Process this query. The results will be send back separately.
			doQuery(information.query, information.callback, information.dataId, NormalExecution);
		}

		thread_sleep(25);
	}

	// Before shutting down, execute the remaining queries with a maximum of fifty. The San
	// Andreas multiplayer server will wait for twelve seconds until loading the gamemode again
	// as it is, which gives us 0.24 seconds per query.
	unsigned int executedQueries = 0;
	while (connection_status_.is_connected && query_queue_.size() > 0 && executedQueries++ < 50) {
		ConnectionMessages::QueryInformation information;
		query_queue_.pop(information);

		// Process the query. No information will be relayed to the gamemode anymore.
		doQuery(information.query, information.callback, information.dataId, SilentExecution);
	}

	// Always gracefully close the connection after we're done.
	if (connection_status_.has_connection_information == true && connection_status_.is_connected == true)
		doClose();
}
