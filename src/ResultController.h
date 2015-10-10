// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include "ScopedMutex.h"
#include <map>

class ResultEntry;

typedef std::map<unsigned int, ResultEntry*> ResultMap;

class ResultController {
public:

	static ResultController* instance() {
		if (instance_ == 0)
			instance_ = new ResultController();

		return instance_;
	}

	ResultController()
	  : result_map_()
	  , latest_result_id_(0)
	  , mutex_()
	{
	}

	unsigned int push(ResultEntry* entry);
	int free(unsigned int resultId);

	ResultEntry* at(unsigned int resultId) {
		ResultMap::iterator entry = result_map_.find(resultId);
		if (entry == result_map_.end())
			return nullptr;

		return entry->second;
	}

private:

	static ResultController* instance_;

	ResultMap result_map_;
	unsigned int latest_result_id_;

	Mutex mutex_;
};
