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
