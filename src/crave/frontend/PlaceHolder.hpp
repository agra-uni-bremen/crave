// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "ConstraintType.hpp"
#include "Variable.hpp"

namespace crave {
// placeholder
/**
 * A Placeholder is needed in foreach constraints for vectors.
 */
struct placeholder_tag {
  explicit placeholder_tag(int id_) : id(id_) {}
  int id;
};

struct placeholder : public Constraint<boost::proto::terminal<placeholder_tag>::type> {
  typedef Constraint<boost::proto::terminal<placeholder_tag>::type> base_type;

  /**
   * constructor
   */
  placeholder() : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(placeholder_tag(new_var_id()))) {}

  /**
   * get id
   * @return id
   */
  int id() const { return boost::proto::value(*this).id; }
};
}  // namespace crave
