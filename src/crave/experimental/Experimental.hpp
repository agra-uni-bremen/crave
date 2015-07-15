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
#include <memory>

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

  std::string kind() override final { return "crv_sequence_item"; }

  bool randomize() override {
    if (!built_) {
      gen_ = std::make_shared<Generator>();
      recursive_build(*gen_);
      built_ = true;
    }
    return gen_->nextCov();
  }

  void goal(crv_covergroup& group) {
    for (auto e : group.bound_var_expr_list()) (*gen_)(e);
    for (auto e : group.uncovered_as_list()) gen_->cover(e);
  }

 protected:
  void request_rebuild() override {
    built_ = false;
    gen_.reset();
    crv_object::request_rebuild();
  }

  void recursive_build(Generator& gen) {
    for (crv_object* obj : children_) {
      if (obj->kind() == "crv_constraint") {
        crv_constraint* cstr = (crv_constraint*)obj;
        if (cstr->active()) gen(cstr->fullname(), cstr->single_expr());
      }
      else if (obj->kind() == "crv_sequence_item") {
        crv_sequence_item* item = (crv_sequence_item*)obj;
        item->recursive_build(gen);
      }
    }
  }

  std::shared_ptr<Generator> gen_;
  bool built_;
};

}  // end namespace crave
