// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Object.hpp"
#include "Expression.hpp"

#include <glog/logging.h>

#include <string>

namespace crave {

class crv_constraint_base : public crv_object {
 public:
  crv_constraint_base() : active_(true) {}

  crv_constraint_base(expression_list list) : list_(list), active_(true) {}

  expression_list const& expr_list() const { return list_; }

  std::string kind() override final { return "crv_constraint"; }

  void activate() {
    if (!active_) {
      active_ = true;
      request_rebuild();
    }
  }

  void deactivate() {
    if (active_) {
      active_ = false;
      request_rebuild();
    }
  }

  bool active() const { return active_; }

  virtual bool soft() const = 0;

 protected:
  expression_list list_;
  bool active_;
};

enum ConstraintType { hard, soft };

template<ConstraintType type>
class crv_constraint_ : public crv_constraint_base {
 public:
  crv_constraint_(crv_object_name) {}

  crv_constraint_(crv_object_name, expression_list list) : crv_constraint_base(list) {}

  crv_constraint_base& operator &= (expression_list list) {
    list_.join(list);
    return *this;
  }

  void operator=(expression_list list) { list_ = list; }

  bool soft() const override { return type == ConstraintType::soft; }
};

typedef crv_constraint_<ConstraintType::hard> crv_constraint;
typedef crv_constraint_<ConstraintType::soft> crv_soft_constraint;

}  // end namespace crave
