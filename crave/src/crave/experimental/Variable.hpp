/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/


#pragma once

#include "VariableBase.hpp"

#include "../frontend/Distribution.hpp"
#include "../ir/UserExpression.hpp"

namespace crave {

/**
 * \ingroup newAPI
 * \brief Creates a distribution_tag to be used directly in constraints from a single weighted range.
 * Convenient function, which calls distribution::create.
 * 
 * \param range a weighted range.
 * \return A distribution_tag of a distribution to be used in constraints.
 */
template <typename T>
distribution_tag<T> make_distribution(weighted_range<T> const& range) {
  return distribution<T>::create(range);
}

/**
 * \ingroup newAPI
 * \brief Creates a distribution_tag to be used directly in constraints from the given ranges.
 * 
 * Convenient function, which should be preferred to chaining calls of distribution::create and distribution::operator().
 * 
 * \param range the first range.
 * \param args the following ranges.
 * \return A distribution_tag of a distribution to be used in constraints.
 */
template <typename T, typename... Args>
distribution_tag<T> make_distribution(weighted_range<T> const& range, Args... args) {
  return make_distribution(args...)(range);
}

/**
 * constructor
 * @param name name of the variable
 */
#define CRV_VARIABLE_COMMON_CONSTRUCTORS(Typename) \
 public:                                           \
  crv_variable(crv_object_name name = "var") {}    \
  crv_variable(const crv_variable& other) : crv_variable_base<Typename>(other) {}

/**
 * assignment operator
 * @param i value for the assignment
 */
#define CRV_VARIABLE_ASSIGNMENT_INTERFACE(Typename)                    \
 public:                                                               \
  crv_variable<Typename>& operator=(const crv_variable<Typename>& i) { \
    this->value = i.value;                                             \
    return *this;                                                      \
  }                                                                    \
  crv_variable<Typename>& operator=(Typename i) {                      \
    this->value = i;                                                   \
    return *this;                                                      \
  }

/**
 * arithmetic operations
 * @param i value for arithmetic operation
 */
#define CRV_VARIABLE_ARITHMETIC_INTERFACE(Typename) \
 public:                                            \
  crv_variable<Typename>& operator++() {            \
    ++(this->value);                                \
    return *this;                                   \
  }                                                 \
  Typename operator++(int) {                        \
    Typename tmp = this->value;                     \
    ++(this->value);                                \
    return tmp;                                     \
  }                                                 \
  crv_variable<Typename>& operator--() {            \
    --(this->value);                                \
    return *this;                                   \
  }                                                 \
  Typename operator--(int) {                        \
    Typename tmp = this->value;                     \
    --(this->value);                                \
    return tmp;                                     \
  }                                                 \
  crv_variable<Typename>& operator+=(Typename i) {  \
    this->value += i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator-=(Typename i) {  \
    this->value -= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator*=(Typename i) {  \
    this->value *= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator/=(Typename i) {  \
    this->value /= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator%=(Typename i) {  \
    this->value %= i;                               \
    return *this;                                   \
  }

/**
 * bitwise operators
 * @param i value for bitwise operation
 */
#define CRV_VARIABLE_BITWISE_INTERFACE(Typename)    \
 public:                                            \
  crv_variable<Typename>& operator&=(Typename i) {  \
    this->value &= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator|=(Typename i) {  \
    this->value |= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator^=(Typename i) {  \
    this->value ^= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator<<=(Typename i) { \
    this->value <<= i;                              \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator>>=(Typename i) { \
    this->value >>= i;                              \
    return *this;                                   \
  }

/*!
 *\ingroup newAPI
 *\brief A randomizable variable of type T in new API.
 * 
 * <p>This class is the type for all randomizable variables of type T.
 * Default the following types of C++ are supported:
 * <ul>
 * <li>bool</li>
 * <li>(unsigned) int</li>
 * <li>(signed|unsigned) char</li>
 * <li>(unsigned) short</li>
 * <li>(unsigned) long</li>
 * <li>(unsigned) long long</li>
 * </ul>
 * You can add support for SystemC Datatypes by including experimental/SystemC.hpp.
 * This adds support for
 * <ul>
 * <li>sc_bv<n></li>
 * <li>sc_(u)int<n></li>
 * </ul>
 * It is also possible to randomize enumerations.
 * To randomize an enumeration, you must previously declare it with the macro \ref CRAVE_BETTER_ENUM </p><p>
 * A crv_variable<T> also supports basic binary and arithmetic operations +=,-=,*=,/=,=,|=,&=,%=,<<=,>>=,^=. 
 * A very important operator is the operator (). 
 * This operator can be used to get a WriteReference which is to be used in the definition of constraints.
 * For details see crave::crv_constraint</p><p>
 * For coverage aspects there is a method named bind() for a crv_variable. 
 * Bind takes another crv_variable as a parameter.
 * If two variables are bound together, they share the same value.
 * </p>
*/ 
template <typename T, typename Enable = void>
class crv_variable {};

/**
 * class for randomisation of a variable
 */
template <typename T>
class crv_variable<T, typename std::enable_if<std::is_integral<T>::value>::type> : public crv_variable_base<T> {
  CRV_VARIABLE_COMMON_CONSTRUCTORS(T);
  CRV_VARIABLE_ASSIGNMENT_INTERFACE(T);
  CRV_VARIABLE_ARITHMETIC_INTERFACE(T);
  CRV_VARIABLE_BITWISE_INTERFACE(T);

 public:
  /**
   * generate random value
   */
  bool randomize() override {
    static distribution<T> dist;
    this->value = dist.nextValue();
    return true;
  }
};

template <typename T>
struct bitsize_traits<crv_variable<T>> : public bitsize_traits<T> {};

}  // namespace crave
