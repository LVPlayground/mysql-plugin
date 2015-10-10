// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "ScopedMutex.h"
#include <queue>

template <typename Type>
class ThreadSafeQueue {
public:

	ThreadSafeQueue()
	  : queue_()
	  , mutex_()
	{
	}

	void push(const Type& data) {
		ScopedMutex mutex(&mutex_);
		queue_.push(data);
	}

	void pop(Type& data) {
		ScopedMutex mutex(&mutex_);
		data = queue_.front();
		queue_.pop();
	}

	unsigned int size() {
		return queue_.size();
	}

private:

	std::queue<Type> queue_;
	Mutex mutex_;

};
