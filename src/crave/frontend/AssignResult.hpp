// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.
#pragma once

#include <string>

namespace crave {

class Constant;

struct AssignResult {
  AssignResult() {}

  virtual ~AssignResult() {}

  virtual void set_value(std::string const& result) = 0;

  virtual Constant to_constant(std::string const& result) const = 0;

  virtual Constant value_as_constant() const = 0;
};
} /* namespace crave */
