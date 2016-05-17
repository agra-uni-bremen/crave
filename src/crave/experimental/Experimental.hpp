// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Object.hpp"
#include "Variable.hpp"
#include "Array.hpp"
#include "Vector.hpp"
#include "Expression.hpp"
#include "Constraint.hpp"
#include "Coverage.hpp"
#include "SequenceItem.hpp"

#include "../backend/Generator.hpp"

#include <vector>
#include <memory>

#include "better-enums/enum.hpp"

namespace crave {

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
    }                                                                                \
    int _to_integral() const { return actual_value(); }                              \
    enum_name _to_enum() const { return enum_name::_from_integral(actual_value()); } \
    const char* _to_string() const { return _to_enum()._to_string(); }                      \
    operator enum_name() const { return _to_enum(); }                                \
                                                                                     \
   private:                                                                          \
    crv_constraint enum_internal{"enum_internal"};                                   \
  };                                                                                 \
  }  // namespace crave
