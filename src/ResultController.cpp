// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "ResultController.h"
#include "ResultEntry.h"

ResultController* ResultController::instance_ = 0;

unsigned int ResultController::push(ResultEntry* entry) {
	ScopedMutex mutex(&mutex_);
	result_map_[++latest_result_id_] = entry;
	return latest_result_id_;
}

int ResultController::free(unsigned int resultId) {
	ScopedMutex mutex(&mutex_);

	ResultMap::iterator entry = result_map_.find(resultId);
	if (entry == result_map_.end())
		return 0;

	delete entry->second;
	result_map_.erase(entry);
	return 1;
}
