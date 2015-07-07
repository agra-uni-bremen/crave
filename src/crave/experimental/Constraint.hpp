// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Object.hpp"
#include "Expression.hpp"
#include "../ir/UserExpression.hpp"

#include <glog/logging.h>

#include <string>
#include <vector>
#include <memory>

namespace crave {

class crv_constraint : public crv_object {
 public:
  crv_constraint(crv_object_name) {}

  crv_constraint(crv_object_name, expression_list list) : list_(list) {}

  template <typename Expr>
  void operator()(Expr expr) {
    list_.add_expr(expr);
  }

  expression single_expr() { return list_.single_expr(); }

  void operator=(expression_list list) { list_ = list; }

  std::string kind() override { return "crv_constraint"; }

  void activate() {
      active_ = true;
      request_rebuild();
  }
  
  void deactivate() {
      active_ = false;
      request_rebuild();
  }
  
  bool active(){return active_;}
  
 private:
  
  expression_list list_;
  bool active_;
};

}  // end namespace crave
