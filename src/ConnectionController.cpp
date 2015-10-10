// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "ConnectionController.h"
#include "ConnectionHost.h"

#include <vector>

ConnectionController::~ConnectionController() {
	ConnectionMap::iterator iter;
	for (iter = connections_.begin(); iter != connections_.end(); ++iter)
		delete (*iter).second;
}

ConnectionId ConnectionController::connect(Script& script, char* hostname, char* username, char* password, char* database, unsigned int port) {
	ConnectionHost* connectionHost = new ConnectionHost(script, ++latest_connection_id_);
	connections_[latest_connection_id_] = connectionHost;
	connectionHost->connect(hostname, username, password, database, port);

	return latest_connection_id_;
}

void ConnectionController::processTick() {
	ConnectionMap::iterator iter;
	for (iter = connections_.begin(); iter != connections_.end(); ++iter)
		(*iter).second->processTick();
}

void ConnectionController::onScriptUnload(Script& script) {
	std::vector<ConnectionMap::iterator> removeVector;
	for (ConnectionMap::iterator iter = connections_.begin(); iter != connections_.end(); ++iter) {
		if ((*iter).second->script() == script)
			removeVector.push_back(iter);
	}

	for (std::vector<ConnectionMap::iterator>::iterator iter = removeVector.begin(); iter != removeVector.end(); ++iter) {
		delete (*iter)->second;
		connections_.erase(*iter);
	}
}
