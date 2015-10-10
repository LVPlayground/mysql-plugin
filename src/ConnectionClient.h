// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "ConnectionMessages.h"
#include "Thread.h"
#include "ThreadSafeQueue.h"

#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>

class ConnectionClient : public Thread, protected ConnectionMessages {
	friend class ConnectionHost;

	struct ConnectionStatus {
		ConnectionInformation information;
		bool has_connection_information;
		bool is_connected;
		int last_attempt, last_ping;

		ConnectionStatus()
		  : information()
		  , has_connection_information(false)
		  , is_connected(false)
		  , last_attempt(0)
		  , last_ping(0)
		{
		}
	};

	enum ExecutionType {
		NormalExecution,
		SilentExecution
	};

public:

	// -------------------------------------------------------------------------
	// Host-thread methods.

	ConnectionClient()
	  : connection_queue_()
	  , query_queue_()
	  , connection_attempt_queue_()
	  , connection_status_()
	  , failed_query_queue_()
	  , succeeded_query_queue_()
	{
		mysql_init(&connection_);
	}

protected:

	// -------------------------------------------------------------------------
	// Client-thread methods.

	virtual void run();

	void doConnect();
	void doClose();

	void doPing();

	void doQuery(const std::string& query, const std::string& callback, unsigned int dataId, ExecutionType execution_type);

private:

	// The following queues may be pushed to by the host, popped from by the client.
	ThreadSafeQueue<ConnectionMessages::ConnectionInformation> connection_queue_;
	ThreadSafeQueue<ConnectionMessages::QueryInformation> query_queue_;
	
	// The following queues may be pushed to by the client, popped from by the host.
	ThreadSafeQueue<ConnectionMessages::ConnectionAttemptResult> connection_attempt_queue_;
	ThreadSafeQueue<ConnectionMessages::FailedQueryResult> failed_query_queue_;
	ThreadSafeQueue<ConnectionMessages::SucceededQueryResult> succeeded_query_queue_;

	// The following members are only to be used by the host thread.
	ConnectionStatus connection_status_;

	// The actual connection to MySQL.
	MYSQL connection_;
};
