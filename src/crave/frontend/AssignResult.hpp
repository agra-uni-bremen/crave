#pragma once

#include <string>

namespace crave {

struct AssignResult {
  AssignResult() {}
  virtual ~AssignResult() {}

  virtual void set_value(std::string const& result) = 0;
};
} /* namespace crave */
