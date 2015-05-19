// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "ConstraintType.hpp"
#include <boost/proto/core.hpp>

// Variable
namespace crave {

template <typename T>
struct Variable;

int new_var_id();

template <typename value_type>
struct var_tag {
  explicit var_tag(int id_) : id(id_) {}
  int id;
};

template <typename value_type_>
struct Variable
    : public Constraint<
          typename boost::proto::terminal<var_tag<value_type_> >::type> {
  typedef Constraint<
      typename boost::proto::terminal<var_tag<value_type_> >::type> base_type;

  Variable()
      : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(
            var_tag<value_type_>(new_var_id()))) {}

  template <typename T>
  explicit Variable(T linkto)
      : Variable() {
    linkto.addVariable(*this);
  }

  typedef value_type_ value_type;

  int id() const { return boost::proto::value(*this).id; }

  typename boost::proto::result_of::make_expr<
      boost::proto::tag::assign, Constraint_Domain,
      Variable<value_type_> const&, value_type_ const&>::type const
  operator=(value_type_ const& e) const {
    return boost::proto::make_expr<boost::proto::tag::assign,
                                   Constraint_Domain>(boost::cref(*this),
                                                      boost::cref(e));
  }
};
}
