#include "../crave/backend/VariableSolver.hpp"

namespace crave {
VariableSolver::VariableSolver(const VariableContainer& vcon, const ConstraintPartition& cp)
    : var_ctn_(), constr_pttn_(cp), solver_(FactoryMetaSMT::getNewInstance()) {
  BOOST_FOREACH(VariableContainer::ReadRefPair const& pair, vcon.read_references) {
    if (constr_pttn_.containsVar(pair.first))
      var_ctn_.read_references.push_back(pair);
  }
  BOOST_FOREACH(VariableContainer::ReadRefPair const& pair, vcon.dist_references) {
    if (constr_pttn_.containsVar(pair.first)) {
      var_ctn_.dist_references.push_back(pair);
      assert(vcon.dist_ref_to_var_map.find(pair.first) != vcon.dist_ref_to_var_map.end());
      var_ctn_.dist_ref_to_var_map[pair.first] = vcon.dist_ref_to_var_map.at(pair.first);
    }  
  }
  BOOST_FOREACH(VariableContainer::WriteRefPair const& pair, vcon.write_references) {
    if (constr_pttn_.containsVar(pair.first))
      var_ctn_.write_references.push_back(pair);
  }
  BOOST_FOREACH(int id, constr_pttn_.supportSet()) {
    assert(vcon.variables.find(id) != vcon.variables.end());
    var_ctn_.variables[id] = vcon.variables.at(id);
  }
}

std::vector<std::vector<std::string> > VariableSolver::getContradictions() const { return contradictions_; }

std::vector<std::string> VariableSolver::getInactiveSofts() const { return inactive_softs_; }
}
