// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "ConstraintType.hpp"
#include "Variable.hpp"

namespace crave {
// ReadReference

template <typename value_type>
struct read_ref_tag : public var_tag<value_type> {
  read_ref_tag(int id_, value_type const& ref_) : var_tag<value_type>(id_), ref(ref_) {}

  value_type const& ref;
};

template <typename value_type_>
struct ReadReference : public Constraint<typename boost::proto::terminal<read_ref_tag<value_type_> >::type> {
  typedef Constraint<typename boost::proto::terminal<read_ref_tag<value_type_> >::type> base_type;
  explicit ReadReference(value_type_ const& ref)
      : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(read_ref_tag<value_type_>(new_var_id(), ref))) {}

  typedef value_type_ value_type;

  int id() const { return boost::proto::value(*this).id; }
};

template <typename T>
typename boost::proto::result_of::make_expr<boost::proto::tag::terminal, Constraint_Domain, read_ref_tag<T> >::type
reference(T const& ref) {
  return boost::proto::make_expr<boost::proto::tag::terminal, Constraint_Domain>(read_ref_tag<T>(new_var_id(), ref));
}
}  // namespace crave
