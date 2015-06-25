#include "../crave/backend/VariableDefaultSolver.hpp"

#include <set>

namespace crave {

bool VariableDefaultSolver::bypass_constraint_analysis = false;

VariableDefaultSolver::VariableDefaultSolver(const VariableContainer& vcon, const ConstraintPartition& cp)
    : VariableSolver(vcon, cp) {
  LOG(INFO) << "Create solver for partition " << constr_pttn_;

  BOOST_FOREACH(ConstraintPtr c, constr_pttn_) {
    if (c->isCover()) continue;  // default solver ignores cover constraints
    if (c->isSoft()) {
      solver_->makeSoftAssertion(*c->expr());
    } else {
      solver_->makeAssertion(*c->expr());
    }
  }

  if (bypass_constraint_analysis) return;
  
  analyseConstraints();

  if (!FactorySolver<CUDD>::isDefined()) return;

  LOG(INFO) << "Create BDD solvers for constraints involving single variable";

  std::set<int> vars_with_dist;
  BOOST_FOREACH(VariableContainer::ReadRefPair & pair, var_ctn_.dist_references) {
    assert(var_ctn_.dist_ref_to_var_map.find(pair.first) != var_ctn_.dist_ref_to_var_map.end());
    vars_with_dist.insert(var_ctn_.dist_ref_to_var_map.at(pair.first));
  }

  std::map<int, ConstraintList> const& svc_map = constr_pttn_.singleVariableConstraintMap();
  BOOST_FOREACH(VariableContainer::WriteRefPair & pair, var_ctn_.write_references) {
    int id = pair.first;
    if (svc_map.find(id) == svc_map.end()) continue;
    if (vars_with_dist.find(id) != vars_with_dist.end()) {
      LOG(INFO) << "  Skip var #" << id << " due to existing distribution constraints";      
      continue;
    }
    SolverPtr bdd_solver(FactoryMetaSMT::getNewInstance(CUDD));
    bdd_solvers_[id] = bdd_solver;
    BOOST_FOREACH(ConstraintPtr c, svc_map.at(id)) {
      bdd_solver->makeAssertion(*c->expr());
    }
    LOG(INFO) << "  BDD solver for var #" << id << " created";
  }
}

void VariableDefaultSolver::analyseConstraints() {
  analyseHards();
  if (contradictions_.empty()) {
    analyseSofts();
    LOG(INFO) << "Partition is solvable with " << inactive_softs_.size() << " soft constraint(s) deactivated:";

    BOOST_FOREACH(std::string & s, inactive_softs_) { LOG(INFO) << " " << s; }
  } else {
    LOG(INFO) << "Partition has unsatisfiable hard constraints:";
    uint cnt = 0;

    BOOST_FOREACH(std::vector<std::string> & vs, contradictions_) {
      LOG(INFO) << "  set #" << ++cnt;

      BOOST_FOREACH(std::string & s, vs) { LOG(INFO) << "   " << s; }
    }
  }
}

bool VariableDefaultSolver::solve() {
  if (!contradictions_.empty()) return false;

  BOOST_FOREACH(VariableContainer::WriteRefPair & pair, var_ctn_.write_references) {
    int id = pair.first;
    if (bdd_solvers_.find(id) == bdd_solvers_.end()) continue;
    SolverPtr bdd_solver = bdd_solvers_[id];
    assert(bdd_solver->solve()); // otherwise, contradiction must have been found! 
    bdd_solver->read(*var_ctn_.variables[id], *pair.second);
    NodePtr value(new Constant(pair.second->value()));
    NodePtr var = var_ctn_.variables[id];
    NodePtr eq(new EqualOpr(var, value));
    solver_->makeSuggestion(*eq);
  }

  BOOST_FOREACH(VariableContainer::ReadRefPair & pair, var_ctn_.read_references) {
    solver_->makeAssumption(*pair.second->expr());
  }

  BOOST_FOREACH(VariableContainer::ReadRefPair & pair, var_ctn_.dist_references) {
    solver_->makeSuggestion(*pair.second->expr());
  }

  if (solver_->solve()) {
    BOOST_FOREACH(VariableContainer::WriteRefPair & pair, var_ctn_.write_references) {
      solver_->read(*var_ctn_.variables[pair.first], *pair.second);
    }
    return true;
  }
  return false;
}

void VariableDefaultSolver::analyseHards() {
  boost::scoped_ptr<metaSMTVisitor> solver(FactoryMetaSMT::getNewInstance());

  std::map<unsigned int, NodePtr> s;
  std::vector<std::string> out;
  std::vector<std::vector<unsigned int> > results;

  BOOST_FOREACH(ConstraintPtr c, constr_pttn_) {
    if (!c->isSoft() && !c->isCover()) {
      s.insert(std::make_pair(s.size(), c->expr()));
      out.push_back(c->name());
    }
  }
  results = solver->analyseContradiction(s);

  BOOST_FOREACH(std::vector<unsigned int> result, results) {
    std::vector<std::string> vec;

    BOOST_FOREACH(unsigned int i, result) { vec.push_back(out[i]); }
    contradictions_.push_back(vec);
  }
}

void VariableDefaultSolver::analyseSofts() {
  std::vector<unsigned int> result = solver_->analyseSofts();
  std::vector<unsigned int>::iterator ite = result.begin();
  unsigned cnt = 0;

  BOOST_FOREACH(ConstraintPtr c, constr_pttn_) {
    if (ite == result.end()) break;
    if (c->isSoft()) {
      if (*ite == cnt) {
        ite++;
        inactive_softs_.push_back(c->name());
      }
      cnt++;
    }
  }
}
}
