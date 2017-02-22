// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once
#include "../frontend/PlaceHolder.hpp"
#include "../frontend/ReadReference.hpp"
#include "../frontend/VectorConstraint.hpp"
#include "../frontend/WriteReference.hpp"

namespace crave {

/**
* stream operator for var_tag
* @param out outputstream
* @param tag to be printed value
* @return outputstream
*/
template <typename OUT, typename value_type>
OUT& operator<<(OUT& out, var_tag<value_type> const& tag) {
  out << "var<" << tag.id << ">";
  return out;
}

/**
* stream operator for read_ref_tag
* @param out outputstream
* @param tag to be printed value
* @return outputstream
*/
template <typename OUT, typename value_type>
OUT& operator<<(OUT& out, read_ref_tag<value_type> const& tag) {
  out << "read_ref<" << tag.id << ">";
  return out;
}

/**
* stream operator for write_ref_tag
* @param out outputstream
* @param tag to be printed value
* @return outputstream
*/
template <typename OUT, typename value_type>
OUT& operator<<(OUT& out, write_ref_tag<value_type> const& tag) {
  out << "write_ref<" << tag.id << ">";
  return out;
}

/**
* stream operator for vector_tag
* @param out outputstream
* @param tag to be printed value
* @return outputstream
*/
template <typename OUT, typename value_type>
OUT& operator<<(OUT& out, vector_tag<value_type> const& tag) {
  out << "vector<" << tag.id_ << ">";
  return out;
}

/**
* stream operator for placeholder_tag
* @param out outputstream
* @param tag to be printed value
* @return outputstream
*/
template <typename OUT>
OUT& operator<<(OUT& out, placeholder_tag const& tag);

// special operators

struct operator_inside {};

struct operator_dist {};

struct operator_if_then {};

struct operator_if_then_else : operator_if_then {};

struct operator_foreach {};

struct operator_unique {};

struct operator_bitslice {};

/**
 * \ingroup operators
 * \brief A special operator for constraining a variable to be within a collection of values.
 * 
 * The first parameter is the variable to be randomized.
 * The second parameter is a collection of values.
 */
boost::proto::terminal<operator_inside>::type const inside = {};

/**
 * \ingroup operators
 * \brief A special operator for distribution constraint specification.
 * 
 * The first parameter is the variable to be randomized.
 * The second parameter is the distributon of values. See crave::distribution for details.
 */
boost::proto::terminal<operator_dist>::type const dist = {};

/**
 * \ingroup operators
 * \brief A special operator for describing implications.
 * 
 * The first parameter of the operator is the antecedent.
 * The second parameter is the consequent.
 * crave::if_then_else also adds an else-part as the third parameter.
 */
boost::proto::terminal<operator_if_then>::type const if_then = {};

/**
 * \ingroup operators
 * \brief A special operator for describing conditional constraints.
 * 
 * See also crave::if_then.
 */
boost::proto::terminal<operator_if_then_else>::type const if_then_else = {};

/**
 * \ingroup operators
 * \brief A special operator to iterate over a symbolic vector.
 * 
 * crave::foreach is an operator for iterating over a symbolic vector and applying constraints to its elements.
 * The first parameter of foreach is a symbolic vector of the CRAVE random vector object.
 * The second parameter of foreach is an expression that should be used as a constraint for each element.
 * You can access the current element using the access operator [] and the placeholder variable crave::_i.
 * Arithmetic operations on the index, e.g. (crave::_i + 1) or c(rave::_i - 1) is also supported.
 */
boost::proto::terminal<operator_foreach>::type const foreach = {};

/**
 * \ingroup operators
 * \brief A special operator to ensure all elements of a vector are unique.
 * 
 * crave::unique is a constraint operator that make sure that no value in a given vector appears more than once.
 * The one and only parameter is a symbolic vector from the CRAVE random vector object.
 */
boost::proto::terminal<operator_unique>::type const unique = {};

/**
 * \ingroup operators
 * \brief A special operator to extract bits from a symbolic variable.
 * 
 * crave::bitslice creates an expression that captures a bit range within the binary representation of a variable.
 * The first parameter of bitslice is the position to begin the slice.
 * The second parameter is the position to end the slice.
 * Both bounds are inclusive.
 * The third parameter is the variable.
 */
boost::proto::terminal<operator_bitslice>::type const bitslice = {};

/**
 * outputstreamoperator for operator_inside
 * @param out outputstream
 * @param tag operator_inside
 * @return outputstream
 */
template <typename OUT>
OUT& operator<<(OUT& out, operator_inside const& tag);

/**
* stream operator for operator_dist
* @param out outputstream
* @param tag to be printed object
* @return outputstream
*/
template <typename OUT>
OUT& operator<<(OUT& out, operator_dist const& tag);

/**
* stream operator for operator_if_then
* @param out outputstream
* @param tag to be printed object
* @return outputstream
*/
template <typename OUT>
OUT& operator<<(OUT& out, operator_if_then const& tag);

/**
* stream operator for operator_if_then_else
* @param out outputstream
* @param tag to be printed object
* @return outputstream
*/
template <typename OUT>
OUT& operator<<(OUT& out, operator_if_then_else const& tag);

/**
* stream operator for operator_foreach
* @param out outputstream
* @param tag to be printed object
* @return outputstream
*/
template <typename OUT>
OUT& operator<<(OUT& out, operator_foreach const& tag);

/**
* stream operator for operator_unique
* @param out outputstream
* @param tag to be printed object
* @return outputstream
*/
template <typename OUT>
OUT& operator<<(OUT& out, operator_unique const& tag);

/**
* stream operator for operator_bitslice
* @param out outputstream
* @param tag to be printed object
* @return outputstream
*/
template <typename OUT>
OUT& operator<<(OUT& out, operator_bitslice const& tag);
}  // namespace crave
