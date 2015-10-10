// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

class DebugPrinter {
public:

	static void Print(char* message, ...);

	static void setEnabled(bool enabled) {
		enabled_ = enabled;
	}

private:

	static bool enabled_;
	static char buffer_[2048];

};
