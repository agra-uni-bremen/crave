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

#include <list>
#include <set>
#include <map>
#include "UserConstraintType.hpp"

namespace crave {

typedef std::shared_ptr<UserConstraint> ConstraintPtr;
typedef std::list<ConstraintPtr> ConstraintList;

struct ConstraintPartition {
  friend struct ConstraintPartitioner;

  ConstraintPartition() {}

  typedef ConstraintList::iterator iterator;
  typedef ConstraintList::const_iterator const_iterator;
  typedef ConstraintList::reference reference;
  typedef ConstraintList::const_reference const_reference;
  typedef ConstraintList::size_type size_type;
  typedef ConstraintList::value_type value_type;

  iterator begin();

  const_iterator begin() const;

  iterator end();

  const_iterator end() const;

  void add(ConstraintPtr c);

  bool containsVar(int id) const;

  std::map<int, ConstraintList> const& singleVariableConstraintMap() const;

  std::set<int> const& supportSet() const;

  template <typename ostream>
  friend ostream& operator<<(ostream& os, const ConstraintPartition& cp);

 private:
  ConstraintList constraints_;
  std::set<int> support_vars_;
  std::map<int, ConstraintList> singleVariableConstraintMap_;
};

template <typename ostream>
ostream& operator<<(ostream& os, const ConstraintPartition& cp);
}  // namespace crave
