// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#pragma once

#include <string>
#include <vector>

class Statement {
 public:
  Statement(const char* query, const char* parameters)
    : query_(query)
    , parameters_(parameters) {
  }

  // Returns a string reference to the unprepared Query.
  const std::string& Query() const { return query_; }

  // Returns a string reference to the expected parameter types.
  const std::string& Parameters() const { return parameters_; }

 private:
  std::string query_;
  std::string parameters_;
};

class StatementRegistry {
  typedef std::vector<Statement> StatementVector;

 public:
  StatementRegistry() { }

  // Creates a new statement with the provided information, inserts it in the statement
  // vector and returns the Id of the newly inserted entry.
  unsigned int Create(const char* query, const char* parameters);

  // Retrieves a pointer to the statement at the given index. Since the statements are
  // stored in a vector, the returned pointer must not be stored anywhere.
  const Statement* At(unsigned int index);

private:
  StatementVector statement_vector_;
};
