// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "Object.hpp"
#include "Expression.hpp"
#include "ConstraintBase.hpp"

#include <glog/logging.h>

#include <string>

namespace crave {

enum ConstraintType { hard, soft };

template<ConstraintType type>
class crv_constraint_ : public crv_constraint_base {
 public:
  crv_constraint_(crv_object_name = (type == ConstraintType::hard ? "hard_cstr" : "soft_cstr")) {}

  crv_constraint_(crv_constraint_ const &) = default;
  
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
    request_rebuild();
    return *this;
  }

  crv_constraint_& operator &= (crv_constraint_ const & c) {
    list_.join(c.list_);
    request_rebuild();
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
