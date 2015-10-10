// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include <string>
#include <vector>

class Statement;

std::string escape_string_parameter(const char* query);

class QueryBuilder {
 public:
  // Builds the query and stores the results in the "query" variable. This
  // method assumes the caller has supplied the right number of parameters.
  // NOTE that this method is strictly *not* thread safe.
  static bool Build(const Statement* statement, const std::vector<std::string>& parameters, std::string& result);

private:
  static std::string buffer_;
};
