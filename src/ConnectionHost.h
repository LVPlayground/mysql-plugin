// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "Script.h"
#include <string>

typedef struct tagAMX AMX;
class ConnectionClient;

class ConnectionHost {
public:

	ConnectionHost(Script& script, unsigned int connection_id);
	~ConnectionHost();

	void connect(const char* hostname, const char* username, const char* password, const char* database, unsigned int port);
	void close();

	void query(const char* query, const char* callback, unsigned int dataId);

	void processTick();

	inline Script& script() {
		return script_;
	}

private:

	unsigned int connection_id_;
	ConnectionClient* client_;
	Script script_;
};
