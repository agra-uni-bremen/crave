// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <vector>

#include "RandBase.hpp"
namespace crave {

/**
 * constructor, destructor and set operator
 */
#define RANDV_COMMON_INTERFACE(Typename)                             \
 public:                                                             \
  randv(rand_obj_base* parent = 0) : randv_base<Typename>(parent) {} \
  randv(const randv& other) : randv_base<Typename>(other) {}         \
  randv<Typename>& operator=(const randv<Typename>& i) {             \
    this->value = i.value;                                           \
    return *this;                                                    \
  }                                                                  \
  randv<Typename>& operator=(Typename i) {                           \
    this->value = i;                                                 \
    return *this;                                                    \
  }

/**
* arithmetic operators
*/
#define RANDV_ARITHMETIC_INTERFACE(Typename) \
 public:                                     \
  randv<Typename>& operator++() {            \
    ++(this->value);                         \
    return *this;                            \
  }                                          \
  Typename operator++(int) {                 \
    Typename tmp = this->value;              \
    ++(this->value);                         \
    return tmp;                              \
  }                                          \
  randv<Typename>& operator--() {            \
    --(this->value);                         \
    return *this;                            \
  }                                          \
  Typename operator--(int) {                 \
    Typename tmp = this->value;              \
    --(this->value);                         \
    return tmp;                              \
  }                                          \
  randv<Typename>& operator+=(Typename i) {  \
    this->value += i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator-=(Typename i) {  \
    this->value -= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator*=(Typename i) {  \
    this->value *= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator/=(Typename i) {  \
    this->value /= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator%=(Typename i) {  \
    this->value %= i;                        \
    return *this;                            \
  }

/**
 * bitwise operators
 */
#define RANDV_BITWISE_INTERFACE(Typename)    \
  randv<Typename>& operator&=(Typename i) {  \
    this->value &= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator|=(Typename i) {  \
    this->value |= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator^=(Typename i) {  \
    this->value ^= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator<<=(Typename i) { \
    this->value <<= i;                       \
    return *this;                            \
  }                                          \
  randv<Typename>& operator>>=(Typename i) { \
    this->value >>= i;                       \
    return *this;                            \
  }

/**
 * other basic randv functions
 */
#define RANDV_PRIM_INTERFACE(Typename)                                                         \
 public:                                                                                       \
  void gather_values(std::vector<int64_t>* ch) { ch->push_back(static_cast<int64_t>(value)); } \
  bool next() {                                                                                \
    static distribution<Typename> dist;                                                        \
    value = dist.nextValue();                                                                  \
    return true;                                                                               \
  }

/**
 * random boolean variable
 */
template <>
struct randv<bool> : public randv_base<bool> {
  RANDV_COMMON_INTERFACE(bool);
  RANDV_PRIM_INTERFACE(bool);
//  RANDV_BITWISE_INTERFACE(bool);
};

/**
 * random integer variables
 */
// for all C/C++ built-in integer types
#define RANDV_INTEGER_TYPE(typename)                     \
  template <>                                            \
  struct randv<typename> : public randv_base<typename> { \
    RANDV_COMMON_INTERFACE(typename);                    \
    RANDV_PRIM_INTERFACE(typename);                      \
    RANDV_ARITHMETIC_INTERFACE(typename);                \
    RANDV_BITWISE_INTERFACE(typename);                   \
  };

RANDV_INTEGER_TYPE(int);
RANDV_INTEGER_TYPE(unsigned int);
RANDV_INTEGER_TYPE(char);
RANDV_INTEGER_TYPE(signed char);
RANDV_INTEGER_TYPE(unsigned char);
RANDV_INTEGER_TYPE(short);
RANDV_INTEGER_TYPE(unsigned short);
RANDV_INTEGER_TYPE(long);
RANDV_INTEGER_TYPE(unsigned long);
RANDV_INTEGER_TYPE(long long);
RANDV_INTEGER_TYPE(unsigned long long);
}  // namespace crave

/*!
 *\ingroup oldAPI
 *\deprecated As all old API entities, it is recommended to use the \ref newAPI "new API". The equivalent in the new API is \ref crave::crv_variable
 *\struct crave::randv
 *\brief A randomizable variable of type T.
 * 
 * <p>This struct is the type for all randomizable variables of type T.
 * Default the following types of C++ are supported:
 * <ul>
 * <li>bool</li>
 * <li>(unsigned) int</li>
 * <li>(signed|unsigned) char</li>
 * <li>(unsigned) short</li>
 * <li>(unsigned) long</li>
 * <li>(unsigned) long long</li>
 * </ul>
 * You can add support for SystemC Datatypes by including SystemC.hpp.
 * This adds support for
 * <ul>
 * <li>sc_bv<n></li>
 * <li>sc_(u)int<n></li>
 * </ul>
 * It is also possible to randomize enumerations.
 * To randomize an enumeration, you must previously declare it with the macro \ref CRAVE_ENUM </p><p>
 * A randv<T> also supports basic binary and arithmetic operations +=,-=,*=,/=,=,|=,&=,%=,<<=,>>=,^=. A very important operator is the operator (). This operator can be used to get a writereference to the randv which is used in the definition of constraints. For details see crave::Constraint</p>
*/
