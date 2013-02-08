#pragma once

#include <string>

namespace crave {

  struct AssignResult {
    AssignResult() { }
    virtual ~AssignResult() { }

    virtual long value() const = 0;
    virtual void set_value( std::string const& result ) = 0;
  };
} /* namespace crave */
