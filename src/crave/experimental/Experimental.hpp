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

#include "better-enums/enum.hpp"

#include <vector>
#include <type_traits>

namespace crave {

template <typename... Exprs> 
bool solve(Exprs... exprs) {
  Generator gen;
  for (auto e : expression_list(exprs...)) gen(e);
  return gen.next();
}

template <class T>
struct is_better_enum : std::false_type {};

template <class T>
struct simple_enum_wrapper {};

template <typename T>
class crv_variable<T, typename std::enable_if<is_better_enum<T>::value>::type> : public crv_variable_base<int> {
  CRV_VARIABLE_ASSIGNMENT_INTERFACE(T);
 public:
  crv_variable(const crv_variable& other) : crv_variable_base<int>(other) {}
  crv_variable(crv_object_name name = "var") {
    enum_internal = {inside(var, std::vector<int>(T::_values().begin(), T::_values().end()))};
  }
  int _to_integral() const { return actual_value(); }
  T _to_enum() const { return T::_from_integral(actual_value()); }
  const char* _to_string() const { return _to_enum()._to_string(); }
  operator T() const { return _to_enum(); }
 private:
  crv_constraint enum_internal{"enum_internal"};
};

template <typename T>
class crv_variable<T, typename std::enable_if<simple_enum_wrapper<T>::defined>::type> : public crv_variable_base<int> {
  CRV_VARIABLE_ASSIGNMENT_INTERFACE(T);
 public:
  crv_variable(const crv_variable& other) : crv_variable_base<int>(other) {}
  crv_variable(crv_object_name name = "var") {
    enum_internal = {inside(var, simple_enum_wrapper<T>().values())};
  }
 private:
  crv_constraint enum_internal{"enum_internal"};
};

}  // end namespace crave

#ifdef __clang__

#define CRAVE_EXPERIMENTAL_ENUM(enum_name, ...)           \
  template <>                                             \
  struct crave::simple_enum_wrapper<enum_name> {          \
    static constexpr bool defined = true;                 \
    std::vector<int> values() { return { __VA_ARGS__ }; } \
  };

#define CRAVE_BETTER_ENUM(enum_name, ...)                      \
  BETTER_ENUM(enum_name, int, __VA_ARGS__);                    \
  template <>                                                  \
  struct crave::is_better_enum<enum_name> : std::true_type {};

#else

#define CRAVE_EXPERIMENTAL_ENUM(enum_name, ...)           \
  namespace crave {                                       \
  template <>                                             \
  struct simple_enum_wrapper<enum_name> {                 \
    static constexpr bool defined = true;                 \
    std::vector<int> values() { return { __VA_ARGS__ }; } \
  }; } // namepsace crave

#define CRAVE_BETTER_ENUM(enum_name, ...)               \
  BETTER_ENUM(enum_name, int, __VA_ARGS__);             \
  namespace crave {                                     \
  template <>                                           \
  struct is_better_enum<enum_name> : std::true_type {}; \
  } // namespace crave

#endif
