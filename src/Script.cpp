// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "Script.h"

#include "DebugPrinter.h"
#include "sdk/plugin.h"
#include <string.h>
#include <string>

const int CallbackNotFoundIndex = -1;

// forward OnConnectionAttempt(connectionHandle, bool: succeeded, server[], username[], errno, error[]);
int Script::OnConnectionAttempt(unsigned int connectionId, bool succeeded, const char* server, const char* username, int error_number, const char* error) {
	if (OnConnectionAttempt_index_ == CallbackNotFoundIndex)
		return 0;

	// Attempt to find the OnConnectionAttempt callback in the script.
	if (OnConnectionAttempt_index_ == 0) {
		if (amx_FindPublic(runtime_, "OnConnectionAttempt", &OnConnectionAttempt_index_) != AMX_ERR_NONE) {
			OnConnectionAttempt_index_ = CallbackNotFoundIndex;
			return 0;
		}
	}

	cell returnValue, stringCell[3], *physicalCell;

	// We've now got the callback. Push the arguments in reverse order.
	amx_PushString(runtime_, &stringCell[0], &physicalCell, error, 0, 0);
	amx_Push(runtime_, error_number);
	amx_PushString(runtime_, &stringCell[1], &physicalCell, username, 0, 0);
	amx_PushString(runtime_, &stringCell[2], &physicalCell, server, 0, 0);
	amx_Push(runtime_, succeeded);
	amx_Push(runtime_, connectionId);

	// Execute and get the return value.
	amx_Exec(runtime_, &returnValue, OnConnectionAttempt_index_);

	// Clean up the strings we pushed to Pawn's stack.
	amx_Release(runtime_, stringCell[0]);
	amx_Release(runtime_, stringCell[1]);
	amx_Release(runtime_, stringCell[2]);

	return returnValue;
}

// forward OnQueryError(connectionId, query[], callback[], errno, error[]);
int Script::OnQueryError(unsigned int connectionId, const char* query, const char* callback, int error_number, const char* error) {
	if (OnQueryError_index_ == CallbackNotFoundIndex)
		return 0;

	// Attempt to find the OnQueryError callback in the script.
	if (OnQueryError_index_ == 0) {
		if (amx_FindPublic(runtime_, "OnQueryError", &OnQueryError_index_) != AMX_ERR_NONE) {
			OnQueryError_index_ = CallbackNotFoundIndex;
			return 0;
		}
	}

	cell returnValue, stringCell[3], *physicalCell;

	// Push the arguments to OnQueryError in reverse order.
	amx_PushString(runtime_, &stringCell[0], &physicalCell, error, 0, 0);
	amx_Push(runtime_, error_number);
	amx_PushString(runtime_, &stringCell[1], &physicalCell, callback, 0, 0);
	amx_PushString(runtime_, &stringCell[2], &physicalCell, query, 0, 0);
	amx_Push(runtime_, connectionId);

	// Execute and get the return value.
	amx_Exec(runtime_, &returnValue, OnQueryError_index_);

	// Clean up the strings we pushed to Pawn's stack.
	amx_Release(runtime_, stringCell[0]);
	amx_Release(runtime_, stringCell[1]);
	amx_Release(runtime_, stringCell[2]);

	return returnValue;
}

// forward MyCallback(resultId, dataId);
int Script::Callback(const char* callbackName, int resultId, int dataId) {
	int callbackIndex = 0;
	if (!strlen(callbackName) || amx_FindPublic(runtime_, callbackName, &callbackIndex) != AMX_ERR_NONE)
		return 0;
	
	cell returnValue;
	
	// Push the two arguments to the user's callback, in reversed order.
	amx_Push(runtime_, dataId);
	amx_Push(runtime_, resultId);

	// Execute and get the return value.
	amx_Exec(runtime_, &returnValue, callbackIndex);

	return returnValue;
}
