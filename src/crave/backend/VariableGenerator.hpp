#pragma once

#include "../ir/Node.hpp"
#include "../ir/UserConstraint.hpp"
#include "../ir/VariableContainer.hpp"
#include "FactoryMetaSMT.hpp"

#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <stdexcept>

namespace crave {

/*
 *
 */
struct VariableSolver {
  friend struct VariableGenerator;

  VariableSolver(VariableContainer& vcon, ConstraintPartition& cp)
      : var_ctn(vcon),
        constr_pttn(cp),
        solver(FactoryMetaSMT::getNewInstance()) {}

  virtual bool solve() = 0;

  template <typename T>
  bool read(Variable<T> const& var, T& value) {
    if (var_ctn.variables.find(var.id()) == var_ctn.variables.end())
      return false;
    if (!constr_pttn.containsVar(var.id())) return false;
    AssignResultImpl<T> result;
    solver->read(*var_ctn.variables[var.id()], result);
    value = result.value();
    return true;
  }

  std::vector<std::vector<std::string> > getContradictions() {
    return contradictions_;
  }

  std::vector<std::string> getInactiveSofts() { return inactive_softs_; }

 protected:
  VariableContainer& var_ctn;
  ConstraintPartition& constr_pttn;
  SolverPtr solver;

  std::vector<std::vector<std::string> > contradictions_;
  std::vector<std::string> inactive_softs_;
};

/*
 *
 */
struct VariableDefaultSolver : VariableSolver {

  VariableDefaultSolver(VariableContainer& vcon, ConstraintPartition& cp)
      : VariableSolver(vcon, cp) {

    LOG(INFO) << "Create solver for partition " << constr_pttn;

    BOOST_FOREACH(ConstraintPtr c, constr_pttn) {
      if (c->isCover()) continue;  // default solver ignores cover constraints
      if (c->isSoft()) {
        solver->makeSoftAssertion(*c->expr());
      } else {
        solver->makeAssertion(*c->expr());
      }
    }
    analyseHards();
    if (contradictions_.empty()) {
      analyseSofts();
      LOG(INFO) << "Partition is solvable with " << inactive_softs_.size()
                << " soft constraint(s) deactivated:";
      BOOST_FOREACH(std::string & s, inactive_softs_)
      LOG(INFO) << " " << s;
    } else {
      LOG(INFO) << "Partition has unsatisfiable hard constraints:";
      uint cnt = 0;
      BOOST_FOREACH(std::vector<std::string> & vs, contradictions_) {
        LOG(INFO) << "  set #" << ++cnt;
        BOOST_FOREACH(std::string & s, vs)
        LOG(INFO) << "   " << s;
      }
    }
  }

  virtual bool solve() {
    if (!contradictions_.empty()) return false;
    BOOST_FOREACH(VariableContainer::ReadRefPair pair, var_ctn.read_references)
    if (constr_pttn.containsVar(pair.first))
      solver->makeAssumption(*pair.second->expr());
    BOOST_FOREACH(VariableContainer::ReadRefPair pair, var_ctn.dist_references)
    if (constr_pttn.containsVar(pair.first))
      solver->makeSuggestion(*pair.second->expr());
    if (solver->solve()) {
      BOOST_FOREACH(VariableContainer::WriteRefPair pair,
                    var_ctn.write_references)
      if (constr_pttn.containsVar(pair.first))
        solver->read(*var_ctn.variables[pair.first], *pair.second);
      return true;
    }
    return false;
  }

 private:
  void analyseHards() {
    boost::scoped_ptr<metaSMTVisitor> solver(FactoryMetaSMT::getNewInstance());

    std::map<unsigned int, NodePtr> s;
    std::vector<std::string> out;
    std::vector<std::vector<unsigned int> > results;

    BOOST_FOREACH(ConstraintPtr c, constr_pttn)
    if (!c->isSoft() && !c->isCover()) {
      s.insert(std::make_pair(s.size(), c->expr()));
      out.push_back(c->name());
    }

    results = solver->analyseContradiction(s);

    BOOST_FOREACH(std::vector<unsigned int> result, results) {
      std::vector<std::string> vec;
      BOOST_FOREACH(unsigned int i, result) { vec.push_back(out[i]); }
      contradictions_.push_back(vec);
    }
  }

  void analyseSofts() {
    std::vector<unsigned int> result = solver->analyseSofts();
    std::vector<unsigned int>::iterator ite = result.begin();
    unsigned cnt = 0;
    BOOST_FOREACH(ConstraintPtr c, constr_pttn) {
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
};

/*
 *
 */
struct VariableCoverageSolver : VariableSolver {

  VariableCoverageSolver(VariableContainer& vcon, ConstraintPartition& cp)
      : VariableSolver(vcon, cp) {

    LOG(INFO) << "Create coverage solver for partition " << constr_pttn;

    BOOST_FOREACH(ConstraintPtr c, constr_pttn) {
      if (c->isSoft())
        continue;  // coverage solver ignores soft constraints for now
      if (!c->isCover()) solver->makeAssertion(*c->expr());
    }
  }

  virtual bool solve() {
    BOOST_FOREACH(ConstraintPtr c, constr_pttn) {
      if (!c->isCover()) continue;
      if (covered_set.find(c->name()) != covered_set.end())
        continue;  // alread covered
      // try solve
      BOOST_FOREACH(VariableContainer::ReadRefPair pair,
                    var_ctn.read_references)
      if (constr_pttn.containsVar(pair.first))
        solver->makeAssumption(*pair.second->expr());
      solver->makeAssumption(*c->expr());
      if (solver->solve()) {
        LOG(INFO) << "Solve partition " << constr_pttn << " hitting constraint "
                  << c->name();
        covered_set.insert(c->name());
        BOOST_FOREACH(VariableContainer::WriteRefPair pair,
                      var_ctn.write_references)
        if (constr_pttn.containsVar(pair.first))
          solver->read(*var_ctn.variables[pair.first], *pair.second);
        return true;
      }
    }
    return false;
  }

 private:
  std::set<std::string> covered_set;
};

/**
 *
 */
struct VariableGenerator {
  typedef boost::shared_ptr<VariableSolver> VarSolverPtr;

  VariableGenerator(VariableContainer& vcon) : var_ctn(vcon), solvers() {}

  virtual void reset(std::vector<ConstraintPartition>& partitions) {
    solvers.clear();
    BOOST_FOREACH(ConstraintPartition & partition, partitions) {
      VarSolverPtr vs(new VariableDefaultSolver(var_ctn, partition));
      solvers.push_back(vs);
    }
  }

  virtual bool solve() {
    BOOST_FOREACH(VarSolverPtr vs, solvers)
    if (!vs->solve()) return false;
    return true;
  }

  template <typename T>
  bool read(Variable<T> const& var, T& value) {
    BOOST_FOREACH(VarSolverPtr vs, solvers)
    if (vs->read(var, value)) return true;
    return false;
  }

  std::vector<std::vector<std::string> > analyseContradiction() {
    std::vector<std::vector<std::string> > str_vec;
    BOOST_FOREACH(VarSolverPtr vs, solvers) {
      std::vector<std::vector<std::string> > c = vs->getContradictions();
      if (!c.empty()) str_vec.insert(str_vec.end(), c.begin(), c.end());
    }
    return str_vec;
  }

  std::vector<std::string> getInactiveSofts() {
    std::vector<std::string> str_vec;
    BOOST_FOREACH(VarSolverPtr vs, solvers) {
      std::vector<std::string> c = vs->getInactiveSofts();
      if (!c.empty()) str_vec.insert(str_vec.end(), c.begin(), c.end());
    }
    return str_vec;
  }

 protected:
  VariableContainer& var_ctn;
  std::vector<VarSolverPtr> solvers;
};

/*
 *
 */
struct VariableCoverageGenerator : VariableGenerator {

  VariableCoverageGenerator(VariableContainer& vcon)
      : VariableGenerator(vcon) {}

  virtual void reset(std::vector<ConstraintPartition>& partitions) {
    solvers.clear();
    BOOST_FOREACH(ConstraintPartition & partition, partitions) {
      VarSolverPtr vs(new VariableCoverageSolver(var_ctn, partition));
      solvers.push_back(vs);
    }
  }

  virtual bool solve() {
    bool res = false;
    BOOST_FOREACH(VarSolverPtr vs, solvers)
    if (vs->solve())
      res = true;  // one solver has hitted an uncovered constraint ->
                   // everything ok, from covered solvers, the last solutions
                   // will be reused.
    return res;
  }
};

}  // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab
