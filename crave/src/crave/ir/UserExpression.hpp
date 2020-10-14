/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/

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
