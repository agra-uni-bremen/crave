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

#include "../crave/ir/ConstraintPartition.hpp"
#include <ostream>

namespace crave {

template <typename ostream>
ostream& operator<<(ostream& os, const ConstraintPartition& cp) {
  os << "[ ";

  for(ConstraintPtr c : cp) { os << c->name() << " "; }
  os << "]";
  os << std::flush;
  return os;
}

template std::ostream& operator<<<std::ostream>(std::ostream& os, const ConstraintPartition& cp);

typedef ConstraintList::iterator iterator;
typedef ConstraintList::const_iterator const_iterator;

iterator ConstraintPartition::begin() { return constraints_.begin(); }

const_iterator ConstraintPartition::begin() const { return constraints_.begin(); }

iterator ConstraintPartition::end() { return constraints_.end(); }

const_iterator ConstraintPartition::end() const { return constraints_.end(); }

void ConstraintPartition::add(ConstraintPtr c) {
  iterator ite = constraints_.begin();
  while (ite != constraints_.end() && ((*ite)->id() > c->id())) ite++;
  constraints_.insert(ite, c);
  // only add hard constraints to singleVariableConstraintMap
  if (!c->isSoft() && !c->isCover() && c->support_vars_.size() == 1) {
    unsigned varID = (*(c->support_vars_.begin()));
    singleVariableConstraintMap_[varID].push_back(c);
  }
}

bool ConstraintPartition::containsVar(int id) const { return support_vars_.find(id) != support_vars_.end(); }

std::set<int> const& ConstraintPartition::supportSet() const { return support_vars_; }
std::map<int, ConstraintList> const& ConstraintPartition::singleVariableConstraintMap() const {
  return singleVariableConstraintMap_;
}
}
