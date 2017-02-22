// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.//
#pragma once

#include "Context.hpp"
#include "Node.hpp"

namespace crave {

struct expr_tag : public Constraint<boost::proto::terminal<NodePtr>::type> {
  typedef Constraint<boost::proto::terminal<NodePtr>::type> base_type;
  explicit expr_tag(NodePtr n) : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(n)) {}
};

typedef boost::proto::result_of::make_expr<boost::proto::tag::terminal, Constraint_Domain, NodePtr>::type expression;

/*!
 * \ingroup operators
 * \brief Creates an expression from a boost::proto expression.
 * 
 * This function allows you to store a constraint expression for later use.
 * @arg boost::proto expression
 * @return expression
 */
template <typename Expr>
expression make_expression(Expr e) {
  static Context ctx(variable_container());
  return boost::proto::make_expr<boost::proto::tag::terminal, Constraint_Domain>(boost::proto::eval(e, ctx));
}

/*!
 * \ingroup operators
 * \brief Converts a value into an expression.
 */
template <typename Integer>
expression value_to_expression(Integer const& i) {
  return boost::proto::make_expr<boost::proto::tag::terminal, Constraint_Domain>(Context::new_value(i));
}

}  // end namespace crave
