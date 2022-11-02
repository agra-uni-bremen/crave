// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "Variable.hpp"

namespace crave {
/**
 * WriteReference<T> is an important internal class for randomization.
 * It contains a symbolic variable to be used in constraints and a pointer to value of type T, which will be updated
 * after constraint solving.
 */
template <typename value_type_>
struct WriteReference : public Variable<value_type_> {
  /**
   * constructor
   * @param ref reverence to value
   */
  explicit WriteReference(value_type_ *ref) : ref_(ref) {}

  typedef value_type_ value_type;

 private:
  value_type *ref_;
};
}  // namespace crave
