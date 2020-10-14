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

#include "Expression.hpp"
#include "Object.hpp"

namespace crave {

struct Generator;

/**
 * \ingroup newAPI
 * \brief Base class for crv_constraint.
 */
class crv_constraint_base : public crv_object {
 protected:
  crv_constraint_base();

 public:
  /**
  * \brief Get all expressions of this constraint.
  * \return The expressions of this constraint as a list.
  */
  virtual expression_list const& expr_list() const = 0;

  std::string obj_kind() const override final;

  /*!
   * \brief Activates this constraint.
   * 
   * An activated constraint will be considered during randomization.
   */
  void activate();

  /*!
   * \brief Deactivates this constraint.
   * 
   * A deactivated constraint will not be considered during randomization.
   */
  void deactivate();

  /*!
   * \brief Returns if the constraint is active.
   * \return true if it is active, false otherwise.
   */
  bool active() const;

  /**
   * \brief Checks if constraint is a soft constraint.
   * \return True if this constraint is a soft constraint, false otherwise
   */
  virtual bool soft() const = 0;

  /**
   * \brief Internal implementation details.
   */
  void recursive_build(Generator& gen) const override;

 protected:
  bool active_;
};
}
