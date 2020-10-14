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

#include "ConstraintBase.hpp"
#include "Expression.hpp"
#include "Object.hpp"

#include <string>

namespace crave {

/*!
 * \ingroup newAPI
 * \brief Possible types of constraints.
 * 
 * For details view crv_constraint_.
 */
enum ConstraintType { hard, soft };

/**
 * \ingroup newAPI
 * \brief Class for constraints in the new API.
 * 
 * <p>
 * A constraint can be either a hard or a soft constraint.
 * A hard constraint must always be satisfied by the generated values.
 * In opposite a soft constraint cane be ignore if otherwise no valid randomization is possible.
 * This may lead to a solution that meets all hard constraints but not all soft constraints.</p><p>
 * A constraint contains at least one expression that describes the constraint.
 * Only if all expressions evaluate to true, the constraint is met. </p>
 */
template <ConstraintType type>
class crv_constraint_ : public crv_constraint_base {
 public:
  /**
   * \brief Constructor using a name for this constraint.
   * 
   * If no name is given, hard_cstr is used for a hard constraint and soft_cstr for a soft constraint.
   * 
   * \param crv_object_name Name of the constraint
   */
  crv_constraint_(crv_object_name = (type == ConstraintType::hard ? "hard_cstr" : "soft_cstr")) {}

  /**
   * \brief Copy constructor.
   * \param & Object to copy from.
   */
  crv_constraint_(crv_constraint_ const&) = default;

  /**
   * \brief Constructor with a list of expressions.
   * 
   * Constructs a constraint with the default name and the given expressions.
   * 
   * \param exprs Constraint expressions
   */
  template <typename... Exprs>
  crv_constraint_(Exprs... exprs) : crv_constraint_() {
    list_ = expression_list(exprs...);
  }

  /**
   * \brief Constructor with a list of expressions and a name.
   * 
   * Constructs a constraint with the given name and the given expressions.
   * 
   * \param name Name of the constraint as a crv_object_name
   * \param expers Constraint expressions
   */
  template <typename... Exprs>
  explicit crv_constraint_(crv_object_name name, Exprs... exprs) : crv_constraint_(name) {
    list_ = expression_list(exprs...);
  }

  /**
   * \brief Constructor with a list of expressions and a name.
   * 
   * Constructs a constraint with the given name and the given expressions.
   * 
   * \param name Name of the constraint as a char array
   * \param expers Constraint expressions
   */
  template <typename... Exprs>
  explicit crv_constraint_(const char* name, Exprs... exprs) : crv_constraint_(crv_object_name(name), exprs...) {}

  /**
   * \brief Assigns another constraint to this constraint.
   * 
   * All expressions from the other constraint are copied into this constraint.
   * Both constraints are the same afterwards.
   * 
   * \param constraint The other constraint
   * \return Reference to this constraint
   */
  crv_constraint_& operator=(crv_constraint_ const& c) {
    list_ = c.list_;
    request_rebuild();
    return *this;
  }

  /**
   * \brief Adds a constraint to this constraint.
   * 
   * This basically creates a conjunction of two constraints and saves the result in this constraint.
   * 
   * \param c The other constraint of the conjunction
   * \return Reference to this constraint
   */
  crv_constraint_& operator&=(crv_constraint_ const& c) {
    list_.join(c.list_);
    request_rebuild();
    return *this;
  }

  bool soft() const override { return type == ConstraintType::soft; }

  expression_list const& expr_list() const override { return list_; }

 protected:
  expression_list list_; /** < list of constraint expressions*/
};

/*!
 * \ingroup newAPI
 * \brief Short type for a CRAVE hard constraint.
 */
typedef crv_constraint_<ConstraintType::hard> crv_constraint;
/*!
 * \ingroup newAPI
 * \brief Short type for a CRAVE soft constraint.
 */
typedef crv_constraint_<ConstraintType::soft> crv_soft_constraint;

}  // end namespace crave
