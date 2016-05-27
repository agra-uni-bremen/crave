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
