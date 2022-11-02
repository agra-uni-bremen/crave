// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

// Variable
namespace crave {

int new_var_id();

/**
 * Variable represents symbolic variables to be used in constraint expressions.
 */
template <typename value_type_>
struct Variable {
  /**
   * constructor
   */
  Variable() : id_(new_var_id()) {}

  typedef value_type_ value_type;

  /**
   * get id
   * @return id
   */
  int id() const { return id_; }

 private:
  int id_;
};
}  // namespace crave
