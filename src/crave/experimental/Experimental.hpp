// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Object.hpp"
#include "Variable.hpp"
#include "Expression.hpp"
#include "Constraint.hpp"
#include "Coverage.hpp"

#include "../backend/Generator.hpp"

#include <vector>

namespace crave {

class crv_sequence_item;

class crv_sequence_item : public crv_object {
 public:
  crv_sequence_item() : gen_(), built_(false) { }

  std::string kind() override { return "crv_sequence_item"; }

  bool randomize() override {
    if (!built_) {
      for (crv_object* obj : children_) {
        if (obj->kind() == "crv_constraint") {
          crv_constraint* cstr = (crv_constraint*) obj;
          gen_(cstr->expr());
        }
      }  
      built_ = true;
    }
    return gen_.next(); 
  }
  
  template <typename Expr>
  void constraint(Expr expr) { gen_(expr); }

 protected:
  Generator gen_;  
  bool built_;
};

}  // end namespace crave
