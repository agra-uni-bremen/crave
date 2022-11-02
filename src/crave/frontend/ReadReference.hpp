// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "Variable.hpp"

namespace crave {
/**
 * ReadReference<T> contains a symbolic variable and a reference to a variable of type T.
 * The value of the symbolic variable will be constrained to be the value of the reference when constrain solving is
 * invoked.
 */
template <typename value_type_>
struct ReadReference : public Variable<value_type_> {
  /**
   * constructor
   * @param ref reverence to value
   */
  explicit ReadReference(value_type_ const& ref) : ref_(ref) {}

  typedef value_type_ value_type;

 private:
  value_type const& ref_;
};

/**
 * \ingroup operators
 * \brief Creates a read reference to a non-CRAVE variable.
 *
 * This operator creates a reference to normal C++ variable.
 * The difference of using var and crave::reference(var) in constraint definition is the value of var that will be
 * used in constraint solving. In the former case, the value at the time of constraint definition will be used. In the
 * latter case, the current value at the time of constraint solving will be used. \param ref Variable to refer to
 * \return ReadReference for constraint solving to this variable
 */
template <typename T>
ReadReference<T> reference(T const& ref) {
  return ReadReference<T>(ref);
}
}  // namespace crave
