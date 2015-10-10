// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "QueryBuilder.h"

#include "StatementRegistry.h"

// For escape_string_parameter()
#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>

#if !defined(WIN32)
  #include <string.h>
#endif

// -----------------------------------------------------------------------------

static char string_parameter_buffer[2048];

std::string escape_string_parameter(const char* query) {
  memset(string_parameter_buffer, 0, sizeof(string_parameter_buffer));
  unsigned int buffer_offset = 0;

  string_parameter_buffer[buffer_offset++] = '"';
  buffer_offset += mysql_escape_string(string_parameter_buffer + 1, query, strlen(query));
  string_parameter_buffer[buffer_offset] = '"';

  return std::string(string_parameter_buffer);
}

// -----------------------------------------------------------------------------

std::string QueryBuilder::buffer_(4096, 0);

bool QueryBuilder::Build(const Statement* statement, const std::vector<std::string>& parameters, std::string& result) {
  result.clear();

  unsigned int parameter_index = 0;
  unsigned int query_length = statement->Query().length();
  const std::string& query = statement->Query();

  std::string::size_type current_position = 0;
  while (true) {
    std::string::size_type position = query.find_first_of('?', current_position);
    if (position == std::string::npos) {
      result.append(query.substr(current_position));
      break;
    }

    result.append(query.substr(current_position, position - current_position));
    if (parameter_index >= parameters.size())
      return false;

    result.append(parameters[parameter_index++]);
    current_position = position + 1;
  }

  return true;
}
