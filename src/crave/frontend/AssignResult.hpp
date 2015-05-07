// Copyright 2014 The CRAVE developers. All rights reserved.
#pragma once

#include <string>

namespace crave {

struct AssignResult {
  AssignResult() {}

  virtual ~AssignResult() {}

  virtual void set_value(std::string const& result) = 0;
};
} /* namespace crave */
