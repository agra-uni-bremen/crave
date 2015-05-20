// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/shared_ptr.hpp>
#include <list>
#include <set>

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

  iterator begin() { return constraints_.begin(); }

  const_iterator begin() const { return constraints_.begin(); }

  iterator end() { return constraints_.end(); }

  const_iterator end() const { return constraints_.end(); }

  void add(ConstraintPtr c) {
    iterator ite = constraints_.begin();
    while (ite != constraints_.end() && ((*ite)->id() > c->id())) ite++;
    constraints_.insert(ite, c);
  }

  bool containsVar(int id) {
    return support_vars_.find(id) != support_vars_.end();
  }

  template <typename ostream>
  friend ostream& operator<<(ostream& os, const ConstraintPartition& cp) {
    os << "[ ";

    BOOST_FOREACH(ConstraintPtr c, cp) { os << c->name() << " "; }
    os << "]";
    os << std::flush;
    return os;
  }

 private:
  ConstraintList constraints_;
  std::set<int> support_vars_;
};
}  // namespace crave
