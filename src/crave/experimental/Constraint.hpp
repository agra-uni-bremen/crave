// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Object.hpp"
#include "Expression.hpp"

#include <glog/logging.h>

#include <string>

namespace crave {

class crv_constraint_base : public crv_object {
 protected:
  crv_constraint_base() : active_(true) {}

 public:
  virtual expression_list const& expr_list() const = 0;

  std::string obj_kind() override final { return "crv_constraint"; }

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
  bool active_;
};

enum ConstraintType { hard, soft };

template<ConstraintType type>
class crv_constraint_ : public crv_constraint_base {
 public:
  crv_constraint_(crv_object_name = (type == ConstraintType::hard ? "hard_cstr" : "soft_cstr")) {}

  template <typename... Exprs>
  crv_constraint_(Exprs... exprs) 
      :  crv_constraint_() {
    list_ = expression_list(exprs...);
  }

  template <typename... Exprs>
  explicit crv_constraint_(crv_object_name name, Exprs... exprs) 
      : crv_constraint_(name) {
    list_ = expression_list(exprs...);    
  }

  template <typename... Exprs>
  explicit crv_constraint_(const char* name, Exprs... exprs) 
      : crv_constraint_(crv_object_name(name), exprs...) {} 

  crv_constraint_& operator = (crv_constraint_ const & c) {
    list_ = c.list_;
    return *this;
  }

  crv_constraint_& operator &= (crv_constraint_ const & c) {
    list_.join(c.list_);
    return *this;
  }

  bool soft() const override { return type == ConstraintType::soft; }

  expression_list const& expr_list() const override { return list_; }

 protected:
  expression_list list_; 
};

typedef crv_constraint_<ConstraintType::hard> crv_constraint;
typedef crv_constraint_<ConstraintType::soft> crv_soft_constraint;

}  // end namespace crave
