// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "Variable.hpp"

namespace crave {
/**
 * class for internal symbolic vector variables
 */
template <typename value_type_>
struct Vector {
  /**
   * constructor
   */
  Vector() : id_(new_var_id()) {}

  typedef value_type_ value_type;

  /**
   * get id of this symbolic vector
   * @return id
   */
  int id() const { return id_; }

  /**
   * number of elements in vector as a symbolic variable to used in constraints
   * @return symbolic size
   */
  const Variable<unsigned int>& size() const { return size_; }

 private:
  int id_;
  Variable<unsigned int> size_;
};
}  // namespace crave
