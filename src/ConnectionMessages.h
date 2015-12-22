// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include <string>

class ConnectionMessages {
public:
	// Definitions for the messages we can pass between the host and client threads.

	struct ConnectionInformation {
		std::string hostname, username, password, database;
		unsigned int port;
	};

	struct QueryInformation {
		std::string query, callback;
		unsigned int dataId;
	};

	struct ConnectionAttemptResult {
		std::string hostname, username, error;
		bool succeeded;
		int error_number;
	};

	struct FailedQueryResult {
		std::string query, callback, error;
		int error_number;
	};

	struct SucceededQueryResult {
		std::string callback;
		int result_id, data_id;
	};

};
