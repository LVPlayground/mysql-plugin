// Copyright 2012 Las Venturas Playground. All rights reserved.
// Use of this source code is governed by the GPLv2 license, a copy of which can
// be found in the LICENSE file.

#include "StatementRegistry.h"

unsigned int StatementRegistry::Create(const char* query, const char* parameters) {
  statement_vector_.push_back(Statement(query, parameters));
  return statement_vector_.size() - 1;
}

const Statement* StatementRegistry::At(unsigned int index) {
  if (index >= statement_vector_.size())
    return nullptr;

  return &statement_vector_[index];
}
