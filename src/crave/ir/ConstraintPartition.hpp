// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/shared_ptr.hpp>
#include <list>
#include <set>
#include <map>
#include "UserConstraintType.hpp"

namespace crave {

typedef boost::shared_ptr<UserConstraint> ConstraintPtr;
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
  
  std::map<int, ConstraintList> const & singleVariableConstraintMap() const;
  
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
