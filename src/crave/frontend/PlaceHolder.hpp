// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "ConstraintType.hpp"

namespace crave {
// placeholder

struct placeholder_tag {
  explicit placeholder_tag(int id_) : id(id_) {}
  int id;
};

struct placeholder : public Constraint<boost::proto::terminal<placeholder_tag>::type> {
  typedef Constraint<boost::proto::terminal<placeholder_tag>::type> base_type;

  placeholder() : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(placeholder_tag(new_var_id()))) {}

  int id() const { return boost::proto::value(*this).id; }
};
}  // namespace crave
