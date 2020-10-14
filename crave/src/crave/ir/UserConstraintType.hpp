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

#include <set>
#include <string>
#include <list>
#include <memory>

#include "Node.hpp"

namespace crave {

class UserConstraint {
  friend struct ConstraintManager;
  friend struct ConstraintPartitioner;
  friend struct ConstraintPartition;

 public:
  typedef NodePtr expression;

  UserConstraint(unsigned const id, expression const expr, std::string const& name, std::set<int> support_vars,
                 bool const soft = false, bool const cover = false, bool const enabled = true);

  virtual ~UserConstraint() {}

  unsigned id() const;

  expression const& expr() const;

  std::string name() const;

  bool isSoft() const;

  bool isCover() const;

  bool isEnabled() const;

  void enable();

  void disable();

  virtual bool isVectorConstraint();

  unsigned complexity() const;

  template <typename ostream>
  friend ostream& operator<<(ostream& os, const UserConstraint& constr);

 protected:
  unsigned id_;
  expression expr_;
  std::string name_;
  std::set<int> support_vars_;
  bool soft_;
  bool cover_;
  bool enabled_;
  unsigned complexity_;
};

template <typename ostream>
ostream& operator<<(ostream& os, const UserConstraint& constr);

typedef std::shared_ptr<UserConstraint> ConstraintPtr;
typedef std::list<ConstraintPtr> ConstraintList;

}  // namespace crave
