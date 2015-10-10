// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#ifdef WIN32
	typedef void* thread_t;
	typedef unsigned long thread_return_type;
	#define thread_call_type __stdcall
#else
	#include <pthread.h>
	typedef pthread_t thread_t;
	typedef void* thread_return_type;
	#define thread_call_type
#endif

class Thread {
public:

	Thread()
	  : is_running_(false)
	  , request_shutdown_(false)
	{
	}

	// Only to be called on the host side.
	void startThread();
	void stopThread();

	bool isRunning() {
		return is_running_;
	}

protected:

	// Will only be called on the client side.
	virtual void run() = 0;
	void thread_sleep(unsigned int milliseconds);

	inline bool shutdown_requested() { return request_shutdown_; }

	int time();
	int timeSpan(int start);

private:

	// Only to be called on the host side.
	void start();
	void stop();

	// Only to be called on the client side.
	static thread_return_type thread_call_type internalRun(void* thread);

private:

	thread_t thread_;
	unsigned int thread_id_;
	bool is_running_;
	bool request_shutdown_;

};
