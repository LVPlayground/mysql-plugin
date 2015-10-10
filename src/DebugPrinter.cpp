// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "DebugPrinter.h"
#include "sdk/plugin.h"

#include <stdio.h>
#include <stdarg.h>

bool DebugPrinter::enabled_ = false;
char DebugPrinter::buffer_[2048];

void DebugPrinter::Print(char* message, ...) {
	if (enabled_ == false)
		return;

	va_list arguments;
	va_start(arguments, message);
	vsnprintf(buffer_, sizeof(buffer_), message, arguments);
	va_end(arguments);

	logprintf("%s", buffer_);
}
