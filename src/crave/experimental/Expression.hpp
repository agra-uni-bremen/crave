// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "../frontend/bitsize_traits.hpp"
#include "../ir/UserExpression.hpp"

#include <vector>

namespace crave {

class expression_list {
 public:
  template <typename... Exprs>
  expression_list(Exprs... exprs) : exprs_() {
    add_exprs(exprs...); 
  }

  void operator=(expression_list const & list) { exprs_ = list.exprs_; }

  template <typename Expr>
  void add_expr(Expr expr) { exprs_.push_back(make_expression(expr)); }

  expression single_expr() {
    expression result = value_to_expression(true);
    for (auto e : exprs_) result = make_expression(result && e);
    return result;
  }
    
 private:
  void add_exprs() { }

  template <typename Expr, typename... Exprs>
  void add_exprs(Expr expr, Exprs... exprs) { 
    add_expr(expr);
    add_exprs(exprs...);
  }

  std::vector<expression> exprs_;
};

template <typename IntegerType>
expression onehot(IntegerType const& x) {
  return crave::make_expression((x > 0) && ((x & (x - 1)) == 0));
}

template <typename IntegerType>
expression bitcount(IntegerType const& x) {
  expression result = value_to_expression(0);
  for (unsigned i = 0; i < bitsize_traits<IntegerType>::value; i++)
    result = crave::make_expression(result + ((x >> i) & 1));
  return result;
}

}  // namespace crave
