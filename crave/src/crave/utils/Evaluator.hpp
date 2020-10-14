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

#include "stdint.h"

#include <boost/proto/eval.hpp>

#include "../frontend/bitsize_traits.hpp"
#include "../ir/visitor/EvalVisitor.hpp"
#include "../ir/UserExpression.hpp"

namespace crave {

template <typename Integer>
Integer get_result(EvalVisitor const& v) {
  return static_cast<Integer>(v.result().value());
}

template <>
bool get_result(EvalVisitor const& v);

class Evaluator {
  typedef EvalVisitor::eval_map eval_map;

 public:
  Evaluator() : assignments_(), visitor_(&assignments_) {}

  template <typename var_type, typename value_type>
  void assign(var_type const& var, value_type const& value) {
    unsigned width = bitsize_traits<typename var_type::value_type>::value;
    bool sign = crave::is_signed<typename var_type::value_type>::value;
    assignments_[var.id()] = Constant(value, width, sign);
  }

  void assign(unsigned id, Constant c);

  template <typename Expr>
  bool evaluate(Expr expr) {
    return evaluate(make_expression(expr));
  }
  bool evaluate(expression const& expr);

  template <typename Integer>
  Integer result() const {
    return get_result<Integer>(visitor_);
  }

 private:
  eval_map assignments_;
  EvalVisitor visitor_;
};

}  // end namespace crave
