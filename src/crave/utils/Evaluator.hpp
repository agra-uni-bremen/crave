// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include "../frontend/bitsize_traits.hpp"
#include "../ir/visitor/EvalVisitor.hpp"
#include "../ir/UserExpression.hpp"

#include <boost/proto/eval.hpp>

namespace crave {

class Evaluator {
  typedef EvalVisitor::eval_map eval_map;

 public:
  Evaluator() : assignments_(), visitor_(assignments_) {}

  template <typename var_type, typename value_type>
  void assign(var_type const& var, value_type const& value) {
    unsigned width = bitsize_traits<typename var_type::value_type>::value;
    bool sign = crave::is_signed<typename var_type::value_type>::value;
    assignments_[var.id()] = Constant(value, width, sign);
  }

  void assign(unsigned id, Constant c) { assignments_[id] = c; }

  template <typename Expr>
  bool evaluate(Expr expr) {
    return evaluate(make_expression(expr));
  }
  bool evaluate(expression const& expr) {
      return visitor_.evaluate(*boost::proto::value(expr));
  }

  template <typename Integer>
  Integer result() const {
    return static_cast<Integer>(visitor_.result().value());
  }
  unsigned long long result() const { return result<unsigned long long>(); }

 private:
  eval_map assignments_;
  EvalVisitor visitor_;
};

}  // end namespace crave
