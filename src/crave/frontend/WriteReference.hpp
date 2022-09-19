// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "ConstraintType.hpp"
#include "Variable.hpp"
// WriteReference
namespace crave {

/**
 * tag class for WriteReference
 */
template <typename value_type>
struct write_ref_tag : public var_tag<value_type> {
  /**
   * constructor
   * @param id_ id of object
   * @param ref_ reference
   */
  write_ref_tag(int id_, value_type *ref_) : var_tag<value_type>(id_), ref(ref_) {}
  value_type *ref;
};

/**
 * WriteReference<T> is an important internal class for randomization.
 * It contains a symbolic variable to be used in constraints and a pointer to value of type T, which will be updated
 * after constraint solving.
 */
template <typename value_type_>
struct WriteReference : public Constraint<typename boost::proto::terminal<write_ref_tag<value_type_> >::type> {
  typedef Constraint<typename boost::proto::terminal<write_ref_tag<value_type_> >::type> base_type;

  /**
   * constructor
   * @param ref reverence to value
   */
  explicit WriteReference(value_type_ *ref)
      : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(write_ref_tag<value_type_>(new_var_id(), ref))) {
  }

  typedef value_type_ value_type;

  /**
   * id of this object
   * @return id
   */
  int id() const { return boost::proto::value(*this).id; }

  /**
   * assignment operator, only to be used in constraints.
   * @param e value for assignment
   * @return boost::proto expression
   */
  typename boost::proto::result_of::make_expr<boost::proto::tag::assign, Constraint_Domain,
                                              WriteReference<value_type_> const &, value_type_ const &>::type const
  operator=(value_type_ const &e) const {
    return boost::proto::make_expr<boost::proto::tag::assign, Constraint_Domain>(boost::cref(*this), boost::cref(e));
  }
};
}  // namespace crave
