#include "../crave/backend/VariableDefaultSolver.hpp"

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
  if (!bypass_constraint_analysis)
    analyseConstraints();
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
