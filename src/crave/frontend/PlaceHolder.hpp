// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "Variable.hpp"

namespace crave {
/**
 * A Placeholder is needed in foreach constraints for vectors.
 */
struct placeholder {
  /**
   * constructor
   */
  placeholder() : id_(new_var_id()) {}

  /**
   * get id
   * @return id
   */
  int id() const { return id_; }

  int id_;
};
}  // namespace crave
