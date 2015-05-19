// Copyright 2014 The CRAVE developers. All rights reserved.//
#pragma once

#include "Context.hpp"
#include "Node.hpp"

namespace crave {

using boost::proto::tag::terminal;
using boost::proto::result_of::make_expr;

struct expr_tag : public Constraint<boost::proto::terminal<NodePtr>::type> {
  typedef Constraint<boost::proto::terminal<NodePtr>::type> base_type;
  explicit expr_tag(NodePtr n)
      : base_type(boost::proto::make_expr<terminal>(n)) {}
};

typedef typename make_expr<terminal, Constraint_Domain, NodePtr>::type
    expression;

template <typename Expr>
expression make_expression(Expr e) {
  static Context ctx(&crave::variables);
  return boost::proto::make_expr<terminal, Constraint_Domain>(
      boost::proto::eval(e, ctx));
}

template <typename Integer>
expression value_to_expression(Integer const& i) {
  return boost::proto::make_expr<terminal, Constraint_Domain>(
      Context::new_value(i));
}

}  // end namespace crave
