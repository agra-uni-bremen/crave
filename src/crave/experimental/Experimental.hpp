// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Object.hpp"
#include "Variable.hpp"
#include "Array.hpp"
#include "Vector.hpp"
#include "Expression.hpp"
#include "Constraint.hpp"
#include "Coverage.hpp"

#include "../backend/Generator.hpp"

#include <vector>
#include <memory>

#include "better-enums/enum.hpp"

namespace crave {

class crv_sequence_item : public crv_object {
 public:
  crv_sequence_item() : gen_(), built_(false) {}

  std::string obj_kind() override final { return "crv_sequence_item"; }

  bool randomize() override {
    if (!built_) {
      gen_ = std::make_shared<Generator>();
      recursive_build(*gen_);
      built_ = true;
    }
    return gen_->nextCov();
  }

  template <typename... Exprs>
  bool randomize_with(Exprs... exprs) {
    // TODO Generator caching
    rand_with_gen_ = std::make_shared<Generator>();
    recursive_build(*rand_with_gen_);
    expression_list list(exprs...);
    for (auto e : list) (*rand_with_gen_)(e);
    return rand_with_gen_->next();
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
      if (obj->obj_kind() == "crv_constraint") {
        crv_constraint_base* cstr = (crv_constraint_base*)obj;
        if (!cstr->active()) continue;
        unsigned cnt = 0;
        if (!cstr->soft()) {
          for (auto e : cstr->expr_list()) 
            gen(cstr->fullname() + "#" + std::to_string(cnt++), e);
        } else {
          for (auto e : cstr->expr_list())
            gen.soft(cstr->fullname() + "#" + std::to_string(cnt++), e);
        }            
      } else if (obj->obj_kind() == "crv_sequence_item") {
        crv_sequence_item* item = (crv_sequence_item*)obj;
        item->recursive_build(gen);
      }
    }
  }

  std::shared_ptr<Generator> gen_;
  std::shared_ptr<Generator> rand_with_gen_;
  bool built_;
};

template <typename... Exprs> 
bool solve(Exprs... exprs) {
  Generator gen;
  for (auto e : expression_list(exprs...)) gen(e);
  return gen.next();
}

}  // end namespace crave

#define __CRAVE_EXPERIMENTAL_INSERT(s, DATA, ELEM) DATA.insert(ELEM);

#define CRAVE_EXPERIMENTAL_ENUM(enum_name, ...)                                \
  namespace crave {                                                            \
  template <>                                                                  \
  class crv_variable<enum_name> : public crv_variable_base<int> {              \
    CRV_VARIABLE_ASSIGNMENT_INTERFACE(enum_name);                              \
                                                                               \
   public:                                                                     \
    crv_variable(const crv_variable& other) : crv_variable_base<int>(other) {} \
    crv_variable(crv_object_name name = "var") {                               \
      std::set<int> s;                                                         \
      BOOST_PP_SEQ_FOR_EACH(__CRAVE_EXPERIMENTAL_INSERT, s, __VA_ARGS__);      \
      enum_internal = {inside(var, s)};                                        \
      if (!parent_) {                                                          \
        throw std::runtime_error("crv_variable<enum> cannot be orphaned");     \
      }                                                                        \
      parent_->children_.push_back(&enum_internal);                            \
    }                                                                          \
                                                                               \
   private:                                                                    \
    crv_constraint enum_internal{"enum_internal"};                             \
  };                                                                           \
  }  // namespace crave

#define CRAVE_BETTER_ENUM(enum_name, ...)                                            \
  BETTER_ENUM(enum_name, int, __VA_ARGS__);                                          \
  namespace crave {                                                                  \
  template <>                                                                        \
  class crv_variable<enum_name> : public crv_variable_base<int> {                    \
    CRV_VARIABLE_ASSIGNMENT_INTERFACE(enum_name);                                    \
                                                                                     \
   public:                                                                           \
    crv_variable(const crv_variable& other) : crv_variable_base<int>(other) {}       \
    crv_variable(crv_object_name name = "var") {                                     \
      std::set<int> s;                                                               \
      for (enum_name value : enum_name::_values()) s.insert(value);                  \
      enum_internal = {inside(var, s)};                                              \
      if (!parent_) {                                                                \
        throw std::runtime_error("crv_variable<enum> cannot be orphaned");           \
      }                                                                              \
      parent_->children_.push_back(&enum_internal);                                  \
    }                                                                                \
    enum_name _to_enum() const { return enum_name::_from_integral(actual_value()); } \
    const char* _to_string() const { _to_enum()._to_string(); }                      \
    operator enum_name() const { return _to_enum(); }                                \
                                                                                     \
   private:                                                                          \
    crv_constraint enum_internal{"enum_internal"};                                   \
  };                                                                                 \
  }  // namespace crave
