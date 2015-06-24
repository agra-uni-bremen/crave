#include "../crave/backend/VariableSolver.hpp"

namespace crave {
VariableSolver::VariableSolver(VariableContainer* vcon, const ConstraintPartition& cp)
    : var_ctn_(), constr_pttn_(cp), solver_(FactoryMetaSMT::getNewInstance()) {
  BOOST_FOREACH(VariableContainer::ReadRefPair & pair, vcon->read_references) {
    if (constr_pttn_.containsVar(pair.first))
      var_ctn_.read_references.push_back(pair);
  }
  BOOST_FOREACH(VariableContainer::ReadRefPair & pair, vcon->dist_references) {
    if (constr_pttn_.containsVar(pair.first))
      var_ctn_.dist_references.push_back(pair);
  }
  BOOST_FOREACH(VariableContainer::WriteRefPair & pair, vcon->write_references) {
    if (constr_pttn_.containsVar(pair.first))
      var_ctn_.write_references.push_back(pair);
  }
  BOOST_FOREACH(unsigned id, constr_pttn_.supportSet()) {
    assert(vcon->variables.find(id) != vcon->variables.end());
    var_ctn_.variables[id] = vcon->variables[id];
  }
}

std::vector<std::vector<std::string> > VariableSolver::getContradictions() const { return contradictions_; }

std::vector<std::string> VariableSolver::getInactiveSofts() const { return inactive_softs_; }
}
