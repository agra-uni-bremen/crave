#pragma once

#include "Object.hpp"
#include "../ir/UserExpression.hpp"

#include <glog/logging.h>

#include <string>
#include <vector>
#include <memory>

namespace crave {

class crv_constraint;

class crv_constraint : public crv_object {
 public:
  crv_constraint(crv_object_name) { }

  template <typename... Exprs>
  crv_constraint(crv_object_name, Exprs... exprs) : exprs_() { 
    add_exprs(exprs...);
  }

  template <typename... Exprs>
  void operator()(Exprs... exprs) {
    exprs_.clear();
    add_exprs(exprs...);
  }

  std::string kind() override { return "crv_constraint"; }

  template <typename Expr>
  void add_expr(Expr expr) { exprs_.push_back(make_expression(expr)); }

  expression expr() {
    expression result = value_to_expression(true);
    for (auto e : exprs_) result = make_expression(result && e);
    return result;
  }

 private:
  void add_exprs() { }

  template <typename Expr, typename... Exprs>
  void add_exprs(Expr expr, Exprs... exprs) { 
    exprs_.push_back(make_expression(expr));
    add_exprs(exprs...);
  }

  std::vector<expression> exprs_;  
};

}  // end namespace crave
