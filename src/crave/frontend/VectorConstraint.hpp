// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Variable.hpp"
#include "ConstraintType.hpp"

namespace crave {
// Vector

template <typename value_type>
struct vector_tag {
  explicit vector_tag(int id) : id_(id) {}
  int id_;
};

template <typename value_type_>
struct Vector
    : public Constraint<
          typename boost::proto::terminal<vector_tag<value_type_> >::type> {
  typedef Constraint<typename boost::proto::terminal<
      vector_tag<value_type_> >::type> base_type;

  Vector()
      : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(
            vector_tag<value_type_>(new_var_id()))) {}

  typedef value_type_ value_type;

  int id() const { return boost::proto::value(*this).id; }

  const Variable<unsigned int>& size() const { return size_; }

 private:
  Variable<unsigned int> size_;
};
}  // namespace crave
