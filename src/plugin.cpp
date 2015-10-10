// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include <algorithm>
#include <string>
#include <vector>

#include "sdk/plugin.h"

#include "ConnectionController.h"
#include "ConnectionHost.h"
#include "DebugPrinter.h"
#include "ResultController.h"
#include "ResultEntry.h"
#include "StatementRegistry.h"
#include "QueryBuilder.h"

#ifdef WIN32
  #define snprintf sprintf_s
#endif

logprintf_t logprintf;

ConnectionController* connectionController = 0;
ResultController* resultController = 0;
StatementRegistry* statementRegistry = 0;

// -----------------------------------------------------------------------------

// native mysql_connect(const hostname[], const username[], const password[], const database[], port = 3306);
// returns: connectionHandle.
static cell AMX_NATIVE_CALL n_mysql_connect(AMX* amx, cell* params) {
	CHECK_PARAMS(5);

	char* hostname, * username, * password, * database;
	amx_StrParam(amx, params[1], hostname);
	amx_StrParam(amx, params[2], username);
	amx_StrParam(amx, params[3], password);
	amx_StrParam(amx, params[4], database);
	unsigned int port = params[5];

	Script script(amx);
	return connectionController->connect(script, hostname, username, password, database, port);
}

// native mysql_close(connectionHandle);
static cell AMX_NATIVE_CALL n_mysql_close(AMX* amx, cell* params) {
	CHECK_PARAMS(1);

	ConnectionHost* connection = connectionController->connection(static_cast<ConnectionId>(params[1]));
	if (connection == nullptr)
    return 0;

	connection->close();
	return 1;
}

// native mysql_query(connectionHandle, query[], callback[], dataId = 0);
static cell AMX_NATIVE_CALL n_mysql_query(AMX* amx, cell* params) {
	CHECK_PARAMS(4);

	ConnectionId connectionId = params[1];
	ConnectionHost* connection = connectionController->connection(connectionId);
	if (connection == nullptr)
		return 0;

	char* query, *callback;
	amx_StrParam(amx, params[2], query);
	amx_StrParam(amx, params[3], callback);
	unsigned int dataId = params[4];

	connection->query(query, callback, dataId);
	return 1;
}

// native mysql_statement_prepare(query[], parameters[]);
static cell AMX_NATIVE_CALL n_mysql_statement_prepare(AMX* amx, cell* params) {
  CHECK_PARAMS(2);

  char* query, *parameters;
  amx_StrParam(amx, params[1], query);
  amx_StrParam(amx, params[2], parameters);

  if (strlen(query) == 0)
    return -1;

  return statementRegistry->Create(query, parameters);
}

// native mysql_statement_execute(connectionHandle, statementId, callback[], dataId, {Float,_}:...);
static cell AMX_NATIVE_CALL n_mysql_statement_execute(AMX* amx, cell* params) {
  if (params[0] < 4 * sizeof(cell)) {
    logprintf("SCRIPT: Bad parameter count (%d < 4): ", params[0]);
    return 0;
  }

  ConnectionHost* connection = connectionController->connection(params[1]);
  const Statement* statement = statementRegistry->At(params[2]);
  if (connection == nullptr || statement == nullptr)
    return 0;

  char* callback;
  amx_StrParam(amx, params[3], callback);

  unsigned int dataId = params[4];
  const int parameterOffset = 4;

  if ((statement->Parameters().length() + parameterOffset) * sizeof(cell) != params[0]) {
    logprintf("[lvp_MySQL] The statement call expected %d parameters, received %d.", statement->Parameters().length() + parameterOffset, params[0] / sizeof(cell));
    logprintf("[lvp_MySQL] Statement: [%s].", statement->Query().c_str());
    return 0;
  }

  std::vector<std::string> parameters;
  const std::string& parameterTypes = statement->Parameters();
  
  char buffer[256];
  cell* address;
  int length = 0;

  for (unsigned int index = 0; index < parameterTypes.length(); ++index) {
    if (amx_GetAddr(amx, params[parameterOffset + 1 + index], &address) != AMX_ERR_NONE)
      return 0; // this case shouldn't happen.

    switch (parameterTypes[index]) {
      case 'i': // integers.
        snprintf(buffer, sizeof(buffer), "%d", *address);
        parameters.push_back(buffer);
        break;

      case 'f': // floats.
        snprintf(buffer, sizeof(buffer), "%.4f", amx_ctof(*address));
        parameters.push_back(buffer);
        break;

      case 's': // strings.
        amx_StrLen(address, &length);
        amx_GetString(buffer, address, 0, std::min(static_cast<unsigned int>(length + 1), sizeof(buffer)));
        buffer[sizeof(buffer)-1] = 0;

        parameters.push_back(escape_string_parameter(buffer));
        break;

      default: // other (unhandled) types.
        logprintf("[lvp_MySQL] Unknown parameter type in statement: '%c'. Cannot execute.", parameterTypes[index]);
        logprintf("[lvp_MySQL] Statement: [%s].", statement->Query().c_str());
        return 0;
    }
  }
  
  std::string query;
  if (QueryBuilder::Build(statement, parameters, query) == false) {
    logprintf("[lvp_MySQL] Unable to build the query, cannot execute this statement.");
    logprintf("[lvp_MySQL] Statement: [%s].", statement->Query().c_str());
    return 0;
  }

  connection->query(query.c_str(), callback, dataId);
  return 1;
}

// native mysql_affected_rows(resultId);
static cell AMX_NATIVE_CALL n_mysql_affected_rows(AMX* amx, cell* params) {
	CHECK_PARAMS(1);

	ResultEntry* entry = resultController->at(static_cast<unsigned int>(params[1]));
	if (entry == nullptr)
    return 0;

	return entry->affected_rows();
}

// native mysql_insert_id(resultId);
static cell AMX_NATIVE_CALL n_mysql_insert_id(AMX* amx, cell* params) {
	CHECK_PARAMS(1);

	ResultEntry* entry = resultController->at(static_cast<unsigned int>(params[1]));
	if (entry == nullptr)
    return 0;

	return entry->insert_id();
}

// native mysql_free_result(resultId);
static cell AMX_NATIVE_CALL n_mysql_free_result(AMX* amx, cell* params) {
	CHECK_PARAMS(1);

	return resultController->free(static_cast<unsigned int>(params[1]));
}

// native mysql_num_rows(resultId);
static cell AMX_NATIVE_CALL n_mysql_num_rows(AMX* amx, cell* params) {
	CHECK_PARAMS(1);

	ResultEntry* entry = resultController->at(static_cast<unsigned int>(params[1]));
	if (entry == nullptr)
    return 0;

	return entry->num_rows();
}

// native mysql_fetch_row(resultId);
static cell AMX_NATIVE_CALL n_mysql_fetch_row(AMX* amx, cell* params) {
	CHECK_PARAMS(1);

	ResultEntry* entry = resultController->at(static_cast<unsigned int>(params[1]));
	if (entry == nullptr)
    return 0;
		
  return entry->fetch_row() ? 1 : 0;
}

// native mysql_fetch_field_int(resultId, field[]);
static cell AMX_NATIVE_CALL n_mysql_fetch_field_int(AMX* amx, cell* params) {
	CHECK_PARAMS(2);

	ResultEntry* entry = resultController->at(static_cast<unsigned int>(params[1]));
	if (entry == nullptr)
		return 0;

	char* fieldName;
	amx_StrParam(amx, params[2], fieldName);

	if (!fieldName || !strlen(fieldName))
		return 0;

	ColumnInfo::ColumnType fieldType = ColumnInfo::UnknownColumnType;
	const RowInfo::FieldValue* fieldValue = entry->fetch_field(fieldName, fieldType);
	
	if (fieldType != ColumnInfo::IntegerColumnType) {
		DebugPrinter::Print("[MySQL] Tried to get column \"%s\" with the wrong data-type, ignored.", fieldName);
		return 0;
	}

	return fieldValue->integer_val;
}

// native Float: mysql_fetch_field_float(resultId, field[]);
static cell AMX_NATIVE_CALL n_mysql_fetch_field_float(AMX* amx, cell* params) {
	CHECK_PARAMS(2);

	ResultEntry* entry = resultController->at(static_cast<unsigned int>(params[1]));
	if (entry == nullptr)
		return 0;

	char* fieldName;
	amx_StrParam(amx, params[2], fieldName);

	if (!fieldName || !strlen(fieldName))
		return 0;

	ColumnInfo::ColumnType fieldType = ColumnInfo::UnknownColumnType;
	const RowInfo::FieldValue* fieldValue = entry->fetch_field(fieldName, fieldType);
	
	if (fieldType != ColumnInfo::FloatColumnType) {
		DebugPrinter::Print("[MySQL] Tried to get column \"%s\" with the wrong data-type, ignored.", fieldName);
		return 0;
	}

	return amx_ftoc(fieldValue->float_val);
}

// native mysql_fetch_field_string(resultId, field[], buffer[], bufferSize = sizeof(buffer));
static cell AMX_NATIVE_CALL n_mysql_fetch_field_string(AMX* amx, cell* params) {
	CHECK_PARAMS(4);

	ResultEntry* entry = resultController->at(static_cast<unsigned int>(params[1]));
	if (entry == nullptr)
		return 0;

	char* fieldName;
	amx_StrParam(amx, params[2], fieldName);

	if (!fieldName || !strlen(fieldName))
		return 0;

	cell* bufferCell;
	amx_GetAddr(amx, params[3], &bufferCell);

	ColumnInfo::ColumnType fieldType = ColumnInfo::UnknownColumnType;
	const RowInfo::FieldValue* fieldValue = entry->fetch_field(fieldName, fieldType);

	if (fieldType != ColumnInfo::StringColumnType) {
		DebugPrinter::Print("[MySQL] Tried to get column \"%s\" with the wrong data-type, ignored.", fieldName);
		return 0;
	}

	amx_SetString(bufferCell, fieldValue->string_val, 0, 0, static_cast<size_t>(params[4]));
	return strlen(fieldValue->string_val);
}

// native mysql_debug(enabled);
static cell AMX_NATIVE_CALL n_mysql_debug(AMX* amx, cell* params) {
	CHECK_PARAMS(1);

  DebugPrinter::setEnabled(1); // we always want to see the following message.
	DebugPrinter::Print("[MySQL] %s debug mode.", params[1] == 1 ? "Enabling" : "Disabling");
	DebugPrinter::setEnabled(params[1] == 1);

	return 1;
}

// -----------------------------------------------------------------------------

AMX_NATIVE_INFO pluginNativeFunctions[] = {
	{ "mysql_debug", n_mysql_debug },

	{ "mysql_connect", n_mysql_connect },
	{ "mysql_close", n_mysql_close },
	{ "mysql_query", n_mysql_query },

  { "mysql_statement_prepare", n_mysql_statement_prepare },
  { "mysql_statement_execute", n_mysql_statement_execute },

	{ "mysql_affected_rows", n_mysql_affected_rows },
	{ "mysql_insert_id", n_mysql_insert_id },
	{ "mysql_free_result", n_mysql_free_result },

	{ "mysql_num_rows", n_mysql_num_rows },
	{ "mysql_fetch_row", n_mysql_fetch_row },
	{ "mysql_fetch_field_int", n_mysql_fetch_field_int },
	{ "mysql_fetch_field_float", n_mysql_fetch_field_float },
	{ "mysql_fetch_field_string", n_mysql_fetch_field_string },
	{ 0, 0 }
};

// -----------------------------------------------------------------------------

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

	connectionController = new ConnectionController();
	resultController = ResultController::instance();
  statementRegistry = new StatementRegistry();

	logprintf("  [LVP] MySQL plugin loaded.");
	return 1;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	if (connectionController != 0) {
		delete connectionController;
		connectionController = 0;
	}

  if (statementRegistry != 0) {
    delete statementRegistry;
    statementRegistry = 0;
  }

	logprintf("  [LVP] MySQL plugin unloaded.");
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
	connectionController->processTick();
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	return amx_Register(amx, pluginNativeFunctions, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	Script script(amx);
	connectionController->onScriptUnload(script);
	return AMX_ERR_NONE;
}
