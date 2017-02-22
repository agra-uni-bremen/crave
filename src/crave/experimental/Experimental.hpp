// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "Array.hpp"
#include "Constraint.hpp"
#include "Coverage.hpp"
#include "Expression.hpp"
#include "Object.hpp"
#include "SequenceItem.hpp"
#include "Variable.hpp"
#include "Vector.hpp"

#include "../backend/Generator.hpp"

#include "better-enums/enum.hpp"

#include <type_traits>
#include <vector>

namespace crave {

/**
 * \ingroup newAPI
 * \brief Solves given list of expressions and tries to calculate a solution.
 * 
 * This function can be used to create an inline solution for constraints.
 * 
 * \return true if a solution is found, false otherwise.
 */
template <typename... Exprs>
bool solve(Exprs... exprs) {
  Generator gen;
  for (auto e : expression_list(exprs...)) gen(e);
  return gen.next();
}

/**
 * type traits for randomizable enum
 */
template <class T>
struct is_better_enum : std::false_type {};

template <class T>
struct simple_enum_wrapper {};

/**
 * Define crv_variable<enum>, to be used in CRAVE_BETTER_ENUM
 */
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

/**
 * Define crv_variable<enum>, to be used in CRAVE_EXPERIMENTAL_ENUM
 */
template <typename T>
class crv_variable<T, typename std::enable_if<simple_enum_wrapper<T>::defined>::type> : public crv_variable_base<int> {
  CRV_VARIABLE_ASSIGNMENT_INTERFACE(T);

 public:
  crv_variable(const crv_variable& other) : crv_variable_base<int>(other) {}
  crv_variable(crv_object_name name = "var") { enum_internal = {inside(var, simple_enum_wrapper<T>().values())}; }

 private:
  crv_constraint enum_internal{"enum_internal"};
};

}  // end namespace crave

/*!
 * \addtogroup newAPI
 *  @{
 */

#ifdef __clang__

/*!
 * \def CRAVE_EXPERIMENTAL_ENUM(enum_name, ...)
 * \brief Old style enum declartion in new API.
 * 
 * This macro allows you to define your enum in the old way but with the new API in background.
 * You need to declare your enum first, then use its name and comma seperated values.
 */
#define CRAVE_EXPERIMENTAL_ENUM(enum_name, ...)         \
  template <>                                           \
  struct crave::simple_enum_wrapper<enum_name> {        \
    static constexpr bool defined = true;               \
    std::vector<int> values() { return {__VA_ARGS__}; } \
  };

/*!
 * \def CRAVE_BETTER_ENUM(enum_name, ...)
 * \brief Simplified enum declaration in new API.
 * 
 * This macros allows you to define a new enum type together with its values.
 * Simply put the desired type name as name and enumerate the possible values with commata seperated afterwards.
 */
#define CRAVE_BETTER_ENUM(enum_name, ...)   \
  BETTER_ENUM(enum_name, int, __VA_ARGS__); \
  template <>                               \
  struct crave::is_better_enum<enum_name> : std::true_type {};

#else

/*!
 * \def CRAVE_EXPERIMENTAL_ENUM(enum_name, ...)
 * \brief Old style enum declartion in new API.
 * 
 * This macro allows you to define your enum in the old way but with the new API in background.
 * You need to declare your enum first, then use its name and comma seperated values.
 */
#define CRAVE_EXPERIMENTAL_ENUM(enum_name, ...)         \
  namespace crave {                                     \
  template <>                                           \
  struct simple_enum_wrapper<enum_name> {               \
    static constexpr bool defined = true;               \
    std::vector<int> values() { return {__VA_ARGS__}; } \
  };                                                    \
  }  // namespace crave

/*!
 * \def CRAVE_BETTER_ENUM(enum_name, ...)
 * \brief Simplified enum declaration in new API.
 * 
 * This macros allows you to define a new enum type together with its values.
 * Simply put the desired type name as name and enumerate the possible values with commata seperated afterwards.
 */
#define CRAVE_BETTER_ENUM(enum_name, ...)               \
  BETTER_ENUM(enum_name, int, __VA_ARGS__);             \
  namespace crave {                                     \
  template <>                                           \
  struct is_better_enum<enum_name> : std::true_type {}; \
  }  // namespace crave
   
#endif

/*! @}*/ 
