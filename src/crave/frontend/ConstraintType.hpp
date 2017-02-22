// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <boost/proto/core.hpp>

namespace crave {
struct Constraint_Domain;

struct Constraint_Grammar : boost::proto::_ {};

/**
 * Internal implementation details.
 * Class for frontend constraint based on boost::proto.
 */
template <typename Expr>
struct Constraint : boost::proto::extends<Expr, Constraint<Expr>, Constraint_Domain> {
  typedef boost::proto::extends<Expr, Constraint<Expr>, Constraint_Domain> base_type;

  /**
   * constructor
   * @param expr
   */
  explicit Constraint(Expr const& expr = Expr()) : base_type(expr) {}

  /**
   * Internal implementation details
   * @param e
   * @return
   */
  template <typename T, typename E2>
  typename boost::proto::result_of::make_expr<boost::proto::tag::assign, Constraint_Domain, Expr, E2>::type operator=(
      E2 e) {
    return boost::proto::make_expr<boost::proto::tag::assign>(*this, e);
  }
};

/**
 * Internal implementation details
 */
struct Constraint_Domain : boost::proto::domain<boost::proto::generator<Constraint>, Constraint_Grammar> {};

/**
 * Internal implementation details
 * @param
 */
template <typename Expr>
void check(Constraint<Expr> const&) {
  BOOST_MPL_ASSERT((boost::proto::matches<Expr, Constraint_Grammar>));
}

/**
 * Internal implementation details
 * @param
 */
template <typename Expr>
void check_not(Constraint<Expr> const&) {
  BOOST_MPL_ASSERT_NOT((boost::proto::matches<Expr, Constraint_Grammar>));
}
}  // namespace crave
