// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#ifdef WIN32
	typedef void* Mutex_t;
#else
	#include <pthread.h>
	typedef pthread_mutex_t Mutex_t;
#endif

class Mutex {
public:

	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:

	Mutex_t mutex_;

};
