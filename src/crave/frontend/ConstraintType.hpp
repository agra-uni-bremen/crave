// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <boost/proto/core.hpp>

namespace crave {
struct Constraint_Domain;

struct Constraint_Grammar : boost::proto::_ {};

template <typename Expr>
struct Constraint : boost::proto::extends<Expr, Constraint<Expr>, Constraint_Domain> {
  typedef boost::proto::extends<Expr, Constraint<Expr>, Constraint_Domain> base_type;

  explicit Constraint(Expr const& expr = Expr()) : base_type(expr) {}

  template <typename T, typename E2>
  typename boost::proto::result_of::make_expr<boost::proto::tag::assign, Constraint_Domain, Expr, E2>::type operator=(
      E2 e) {
    return boost::proto::make_expr<boost::proto::tag::assign>(*this, e);
  }
};

struct Constraint_Domain : boost::proto::domain<boost::proto::generator<Constraint>, Constraint_Grammar> {};

template <typename Expr>
void check(Constraint<Expr> const&) {
  BOOST_MPL_ASSERT((boost::proto::matches<Expr, Constraint_Grammar>));
}

template <typename Expr>
void check_not(Constraint<Expr> const&) {
  BOOST_MPL_ASSERT_NOT((boost::proto::matches<Expr, Constraint_Grammar>));
}
}  // namespace crave
