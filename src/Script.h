// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

typedef struct tagAMX AMX;

class Script {
public:

	explicit Script(AMX* runtime)
	  : runtime_(runtime)
	  , OnConnectionAttempt_index_(0)
	  , OnQueryError_index_(0)
	{
	}

	// Allows to Script objects to be compared with each other.
	inline bool operator==(Script& script) {
		return runtime_ == script.runtime_;
	}

public:

	// forward OnConnectionAttempt(connectionId, bool: succeeded, server[], username[], errno, error[]);
	int OnConnectionAttempt(unsigned int connectionId, bool succeeded, const char* server, const char* username, int error_number, const char* error);

	// forward OnQueryError(connectionId, query[], callback[], errno, error[]);
	int OnQueryError(unsigned int connectionId, const char* query, const char* callback, int error_number, const char* error);

	// forward MyCallback(resultId, dataId);
	int Callback(const char* callbackName, int resultId, int dataId);

private:

	// Maintain the position of the callbacks inside the scripts -- this is initialized lazily.
	int OnConnectionAttempt_index_;
	int OnQueryError_index_;

	// Store the actual script that we're dealing with as well.
	AMX* runtime_;
};
