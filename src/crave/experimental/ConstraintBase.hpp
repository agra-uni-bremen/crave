// Copyright 2014 The CRAVE developers. All rights reserved.
#pragma once

#include "Object.hpp"
#include "Expression.hpp"

namespace crave {

struct Generator;

class crv_constraint_base : public crv_object {
 protected:
  crv_constraint_base();

 public:
  virtual expression_list const& expr_list() const = 0;

  std::string obj_kind() const override final;

  void activate();

  void deactivate();

  bool active() const;

  virtual bool soft() const = 0;

  void recursive_build(Generator& gen) const override;

 protected:
  bool active_;
};

}
