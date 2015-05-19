// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "ConstraintType.hpp"
#include "Variable.hpp"
// WriteReference
namespace crave {

template <typename value_type>
struct write_ref_tag : public var_tag<value_type> {
  write_ref_tag(int id_, value_type& ref_) :
  var_tag<value_type>(id_), ref(ref_) {}
  value_type& ref;
};

template <typename value_type_>
struct WriteReference :
public Constraint<typename boost::proto::terminal<write_ref_tag<value_type_> >::type> {
  typedef Constraint<typename boost::proto::terminal<write_ref_tag<value_type_> >::type> base_type;

  explicit WriteReference(value_type_& ref)
      : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(write_ref_tag<value_type_>(new_var_id(), ref))) {
  }

  typedef value_type_ value_type;

  int id() const { return boost::proto::value(*this).id; }

  typename boost::proto::result_of::make_expr<boost::proto::tag::assign, Constraint_Domain,
                                              WriteReference<value_type_> const&, value_type_ const&>::type const
  operator=(value_type_ const& e) const {
    return boost::proto::make_expr<boost::proto::tag::assign, Constraint_Domain>(boost::cref(*this), boost::cref(e));
  }
};
}  // namespace crave
