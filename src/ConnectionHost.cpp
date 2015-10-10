// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "ConnectionHost.h"

#include "ConnectionClient.h"

ConnectionHost::ConnectionHost(Script& script, unsigned int connection_id)
	: connection_id_(connection_id)
	, script_(script)
{
	client_ = new ConnectionClient();
	client_->startThread();
}

ConnectionHost::~ConnectionHost() {
	if (!client_)
		return;

	client_->stopThread();
	delete client_;
	client_ = 0;
}

void ConnectionHost::connect(const char* hostname, const char* username, const char* password, const char* database, unsigned int port) {
	ConnectionMessages::ConnectionInformation request;
	request.hostname = hostname;
	request.username = username;
	request.password = password;
	request.database = database;
	request.port = port;

	// This should only be called once, for the initial connection being made.
	client_->connection_queue_.push(request);
}

void ConnectionHost::close() {
	client_->stopThread();
}

void ConnectionHost::query(const char* query, const char* callback, unsigned int dataId) {
	ConnectionMessages::QueryInformation request;
	request.query = query;
	request.callback = callback;
	request.dataId = dataId;

	client_->query_queue_.push(request);
}

void ConnectionHost::processTick() {
	if (client_->connection_attempt_queue_.size() > 0) {
		ConnectionMessages::ConnectionAttemptResult result;
		client_->connection_attempt_queue_.pop(result);

		// Callback to the gamemode: OnConnectionAttempt().
		script_.OnConnectionAttempt(connection_id_, result.succeeded, result.hostname.c_str(), result.username.c_str(), result.error_number, result.error.c_str());
	}

	if (client_->failed_query_queue_.size() > 0) {
		ConnectionMessages::FailedQueryResult result;
		client_->failed_query_queue_.pop(result);

		// Callback to the gamemode: OnQueryError().
		script_.OnQueryError(connection_id_, result.query.c_str(), result.callback.c_str(), result.error_number, result.error.c_str());
	}

	if (client_->succeeded_query_queue_.size() > 0) {
		ConnectionMessages::SucceededQueryResult result;
		client_->succeeded_query_queue_.pop(result);

		// Per-query success callback to the gamemode.
		script_.Callback(result.callback.c_str(), result.result_id, result.data_id);
	}
}
