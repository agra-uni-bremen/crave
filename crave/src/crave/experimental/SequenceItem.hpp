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

#include "Coverage.hpp"
#include "Expression.hpp"
#include "Object.hpp"

namespace crave {
struct Generator;

/**
 * \ingroup newAPI
 * \brief Base class for all randomizable objects of CRAVE.
 * 
 * All classes whose objects should be randomized with CRAVE must extends this class. 
 * It contains basic functionality and data necessary for randomzation.
 * You can define as members crv_variable to be randomized and crv_constraint for constraints to be randomized.
 */
class crv_sequence_item : public crv_object {
 public:
   /*!
    * \brief Empty constructor.
    */
  crv_sequence_item();

  /**
   * \brief Copy constructor.
   * \param other The object to be copied from.
   */
  crv_sequence_item(crv_sequence_item const& other);

  std::string obj_kind() const override final;

  bool randomize() override;

  /**
   * \brief Randomize variables with extra inline constraints.
   * 
   * Adds more constraints that should be used to find the next solution for the random variables.
   * This methods works like randomize() except it takes further inline constraints into consideration.
   * 
   * \param exprs extra constraints to use for constraint solving
   * \return true if a solution is found, false otherwise.
   */
  template <typename... Exprs>
  bool randomize_with(Exprs... exprs) {
    expression_list list(exprs...);
    return randomize_with_expr_list(list);
  }

  /**
   * \brief Hints CRAVE to cover a given covergroup.
   * 
   * This method can be used to hint CRAVE which coverage goals exist.
   * CRAVE tries to find a solution for all unhit crv_coverpoint that are inside the given crv_covergroup.
   * 
   * \param group The crv_covergroup that should be fully covered by CRAVE.
   */
  void goal(crv_covergroup& group);

 protected:
  void request_rebuild() override;

  bool randomize_with_expr_list(expression_list const&);
  std::shared_ptr<Generator> gen_;
  std::shared_ptr<Generator> rand_with_gen_;
  bool built_;
  bool cloned_;
};
}
