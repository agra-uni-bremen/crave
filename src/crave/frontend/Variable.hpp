// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <boost/proto/core.hpp>

#include "ConstraintType.hpp"

// Variable
namespace crave {

template <typename T>
struct Variable;

int new_var_id();

/**
 * tag for variables
 */
template <typename value_type>
struct var_tag {
  /**
   * constructor
   * @param id id of this object
   */
  explicit var_tag(int id_) : id(id_) {}
  int id;
};

/**
 * Variable represents symbolic variables to be used in constraint expressions.
 */
template <typename value_type_>
struct Variable : public Constraint<typename boost::proto::terminal<var_tag<value_type_> >::type> {
  typedef Constraint<typename boost::proto::terminal<var_tag<value_type_> >::type> base_type;

  /**
   * constructor
   */
  Variable() : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(var_tag<value_type_>(new_var_id()))) {}

  typedef value_type_ value_type;

  /**
   * get id
   * @return id
   */
  int id() const { return boost::proto::value(*this).id; }

  /**
   * assignment operator, similar to WriteReference::operator=
   */
  typename boost::proto::result_of::make_expr<boost::proto::tag::assign, Constraint_Domain,
                                              Variable<value_type_> const&, value_type_ const&>::type const
  operator=(value_type_ const& e) const {
    return boost::proto::make_expr<boost::proto::tag::assign, Constraint_Domain>(boost::cref(*this), boost::cref(e));
  }
};
}  // namespace crave
