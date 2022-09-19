// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <list>
#include <map>
#include <set>

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
