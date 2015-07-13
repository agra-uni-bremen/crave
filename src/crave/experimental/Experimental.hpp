// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Object.hpp"
#include "Variable.hpp"
#include "Array.hpp"
#include "Expression.hpp"
#include "Constraint.hpp"
#include "Coverage.hpp"

#include "../backend/Generator.hpp"

#include <vector>

#define CRV_VARIABLE(type, name) \
  crave::crv_variable<type> name { #name }

#define CRV_ARRAY(type, size, name) \
  crave::crv_array<type, size> name { #name }

#define CRV_CONSTRAINT(name, ...) \
  crave::crv_constraint name { #name, __VA_ARGS__ }

#define CRV_COVERPOINT(name, ...) \
  crave::crv_coverpoint name { #name }

namespace crave {

class crv_sequence_item : public crv_object {
 public:
  crv_sequence_item() : gen_(), built_(false) {}

  std::string kind() override { return "crv_sequence_item"; }

  bool randomize() override {
    if (!built_) {
      for (crv_object* obj : children_) {
        if (obj->kind() == "crv_constraint") {
          crv_constraint* cstr = (crv_constraint*)obj;
          gen_(cstr->name(),cstr->single_expr());
          if(!cstr->active())
          {
              gen_.disableConstraint(cstr->name());
          }
        }
      }
      built_ = true;
    }
    if (rebuild_) {
        LOG(INFO) << "Rebuild!" << std::endl;
      for (crv_object* obj : children_) {
        if (obj->kind() == "crv_constraint") {
          crv_constraint* cstr = (crv_constraint*)obj;
          if(cstr->active())
          {
            gen_.enableConstraint(cstr->name());
          }
          else
          {
              gen_.disableConstraint(cstr->name());
          }
        }
      }
      rebuild_ = true;
    }
    return gen_.nextCov();
  }

  void goal(crv_covergroup& group) {
    for (auto e : group.bound_var_expr_list()) gen_(e);
    for (auto e : group.uncovered_as_list()) gen_.cover(e);
  }

 protected:
     
     virtual void request_rebuild()
     {
         rebuild_ = true;
         
         LOG(INFO) << this->name() << ":" << "request_rebuild" << std::endl;
         LOG(INFO) << this->name() << ":"  << "set rebuild_ to true" << std::endl;
     }
     
  Generator gen_;
  bool built_;
  bool rebuild_;
};

}  // end namespace crave
