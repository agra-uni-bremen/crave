// Copyright 2014 The CRAVE developers. All rights reserved.
#pragma once

#include "Object.hpp"
#include "Expression.hpp"

namespace crave {

class crv_constraint_base : public crv_object {
 protected:
  crv_constraint_base() : active_(true) {}

 public:
  virtual expression_list const& expr_list() const = 0;

  std::string obj_kind() const override final { return "crv_constraint"; }

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

}