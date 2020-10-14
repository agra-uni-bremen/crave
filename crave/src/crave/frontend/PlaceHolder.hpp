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

#include "ConstraintType.hpp"
#include "Variable.hpp"

namespace crave {
// placeholder
/**
 * A Placeholder is needed in foreach constraints for vectors.
 */
struct placeholder_tag {
  explicit placeholder_tag(int id_) : id(id_) {}
  int id;
};

struct placeholder : public Constraint<boost::proto::terminal<placeholder_tag>::type> {
  typedef Constraint<boost::proto::terminal<placeholder_tag>::type> base_type;

  /**
   * constructor
   */
  placeholder() : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(placeholder_tag(new_var_id()))) {}

  /**
   * get id
   * @return id
   */
  int id() const { return boost::proto::value(*this).id; }
};
}  // namespace crave
