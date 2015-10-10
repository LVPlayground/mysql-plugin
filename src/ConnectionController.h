// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include <map>

class ConnectionHost;
class Script;

typedef unsigned int ConnectionId;
typedef std::map<ConnectionId, ConnectionHost*> ConnectionMap;

class ConnectionController {
public:
	ConnectionController()
		: connections_()
		, latest_connection_id_(0)
	{
	}

	~ConnectionController();

	ConnectionId connect(Script& owningScript, char* hostname, char* username, char* password, char* database, unsigned int port = 3306);

	void onScriptUnload(Script& script);

	inline ConnectionHost* connection(ConnectionId connectionId) {
		if (connections_.find(connectionId) == connections_.end())
			return nullptr;

		return connections_[connectionId];
	}

	void processTick();

private:

	ConnectionMap connections_;
	unsigned int latest_connection_id_;
};
