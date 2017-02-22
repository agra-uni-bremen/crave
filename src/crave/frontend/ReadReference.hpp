// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "ConstraintType.hpp"
#include "Variable.hpp"

namespace crave {
// ReadReference
/**
 * tag class for read_ref
 */
template <typename value_type>
struct read_ref_tag : public var_tag<value_type> {
  read_ref_tag(int id_, value_type const& ref_) : var_tag<value_type>(id_), ref(ref_) {}

  value_type const& ref;
};

/**
 * ReadReference<T> contains a symbolic variable and a reference to a variable of type T.
 * The value of the symbolic variable will be constrained to be the value of the reference when constrain solving is invoked.
 */
template <typename value_type_>
struct ReadReference : public Constraint<typename boost::proto::terminal<read_ref_tag<value_type_> >::type> {
  typedef Constraint<typename boost::proto::terminal<read_ref_tag<value_type_> >::type> base_type;

  /**
   * constructor
   * @param ref reference to object
   */
  explicit ReadReference(value_type_ const& ref)
      : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(read_ref_tag<value_type_>(new_var_id(), ref))) {}

  typedef value_type_ value_type;

  /**
   * get id
   * @return id
   */
  int id() const { return boost::proto::value(*this).id; }
};

/**
 * \ingroup operators
 * \brief Creates a read reference to a non-CRAVE variable.
 * 
 * This operator creates a reference to normal C++ variable.
 * The difference of using var and crave::reference(var) in constraint definition is the value of var that will be used in constraint solving.
 * In the former case, the value at the time of constraint definition will be used.
 * In the latter case, the current value at the time of constraint solving will be used.
 * \param ref Variable to refer to
 * \return ReadReference for constraint solving to this variable
 */
template <typename T>
typename boost::proto::result_of::make_expr<boost::proto::tag::terminal, Constraint_Domain, read_ref_tag<T> >::type
reference(T const& ref) {
  return boost::proto::make_expr<boost::proto::tag::terminal, Constraint_Domain>(read_ref_tag<T>(new_var_id(), ref));
}
}  // namespace crave
