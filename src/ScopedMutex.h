// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "Mutex.h"

class ScopedMutex {
public:

	ScopedMutex(Mutex* mutex)
	  : mutex_(mutex) {
		mutex_->lock();
	}

	~ScopedMutex() {
		mutex_->unlock();
	}

private:

	Mutex* mutex_;
};
