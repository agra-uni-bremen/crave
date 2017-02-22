// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "ConstraintType.hpp"
#include "Variable.hpp"

namespace crave {
// Vector

/**
 * tag class for vector
 */
template <typename value_type>
struct vector_tag {
  /**
   * constructor
   * @param id id of this object
   */
  explicit vector_tag(int id) : id_(id) {}
  int id_;
};

/**
 * class for internal symbolic vector variables
 */
template <typename value_type_>
struct Vector : public Constraint<typename boost::proto::terminal<vector_tag<value_type_> >::type> {
  typedef Constraint<typename boost::proto::terminal<vector_tag<value_type_> >::type> base_type;

  /**
   * constructor
   */
  Vector() : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(vector_tag<value_type_>(new_var_id()))) {}

  typedef value_type_ value_type;

  /**
   * get id of this symbolic vector
   * @return id
   */
  int id() const { return boost::proto::value(*this).id_; }

  /**
   * number of elements in vector as a symbolic variable to used in constraints
   * @return symbolic size
   */
  const Variable<unsigned int>& size() const { return size_; }

 private:
  Variable<unsigned int> size_;
};
}  // namespace crave
