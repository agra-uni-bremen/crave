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

struct VariableSolver {
  friend struct VariableGenerator;

  VariableSolver(VariableContainer& vcon, ConstraintPartition& cp)
  : vcon_(vcon), cp_(cp), solver_(FactoryMetaSMT::getNewInstance()) {  }

  virtual bool solve() = 0;

  template<typename T>
  bool read(Variable<T> const &var, T& value) {
    if (vcon_.variables.find(var.id()) == vcon_.variables.end()) return false;
    if (!cp_.contains_var(var.id())) return false;
    AssignResultImpl<T> result;
    solver_->read(*vcon_.variables[var.id()], result);
    value = result.value();
    return true;
  }

  std::vector<std::vector<std::string> > analyse_contradiction() {
    return contradictions_;
  }

  std::vector<std::string> get_inactive_softs() {
    return inactive_softs_;
  }

protected:
  VariableContainer& vcon_;
  ConstraintPartition& cp_;
  SolverPtr solver_;

  std::vector<std::vector<std::string> > contradictions_;
  std::vector<std::string> inactive_softs_;
};


struct VariableDefaultSolver : VariableSolver {

  VariableDefaultSolver(VariableContainer& vcon, ConstraintPartition& cp)
  : VariableSolver(vcon, cp) {

    LOG(INFO) << "Create solver for partition " << cp_;

    BOOST_FOREACH(ConstraintPtr c, cp_) {
      if (c->is_cover()) continue; // default solver ignores cover constraints
      if (c->is_soft()) {
        solver_->makeSoftAssertion(*c->get_expression());
      } else {
        solver_->makeAssertion(*c->get_expression());
      }
    }
    analyse_hards();
    if (contradictions_.empty()) {
      analyse_softs();
      LOG(INFO) << "Partition is solvable with " << inactive_softs_.size()
                << " soft constraint(s) deactivated:";
      BOOST_FOREACH(std::string& s, inactive_softs_) 
        LOG(INFO) << " " << s;
    }
    else {
      LOG(INFO) << "Partition has unsatisfiable hard constraints:";
      uint cnt = 0;
      BOOST_FOREACH(std::vector<std::string>& vs, contradictions_) {
        LOG(INFO) << "  set #" << ++cnt;
        BOOST_FOREACH(std::string& s, vs)
          LOG(INFO) << "   " << s;
      }
    }
  }

  virtual bool solve() {
    if (!contradictions_.empty()) 
      return false;
    BOOST_FOREACH(VariableContainer::ReadRefPair pair, vcon_.read_references)
      if (cp_.contains_var(pair.first))
        solver_->makeAssumption(*pair.second->expr());
    BOOST_FOREACH(VariableContainer::ReadRefPair pair, vcon_.dist_references)
      if (cp_.contains_var(pair.first))
        solver_->makeSuggestion(*pair.second->expr());
    if (solver_->solve()) { 
      BOOST_FOREACH(VariableContainer::WriteRefPair pair, vcon_.write_references)
        if (cp_.contains_var(pair.first))
          solver_->read(*vcon_.variables[pair.first], *pair.second);
      return true;
    }
    return false;
  }

private:
  void analyse_hards() {
    boost::scoped_ptr<metaSMTVisitor> solver(FactoryMetaSMT::getNewInstance());

    std::map<unsigned int, NodePtr> s;
    std::vector<std::string> out;
    std::vector<std::vector<unsigned int> > results;

    BOOST_FOREACH(ConstraintPtr c, cp_)
      if (!c->is_soft() && !c->is_cover()) {
        s.insert(std::make_pair(s.size(), c->get_expression()));
        out.push_back(c->get_name());
      }

    results = solver->analyseContradiction(s);

    BOOST_FOREACH(std::vector<unsigned int> result, results) {
      std::vector<std::string> vec;
      BOOST_FOREACH(unsigned int i, result)
      {
        vec.push_back(out[i]);
      }
      contradictions_.push_back(vec);
    }
  }

  void analyse_softs() {
    std::vector<unsigned int> result = solver_->analyseSofts();
    std::vector<unsigned int>::iterator ite = result.begin();
    unsigned cnt = 0;
    BOOST_FOREACH(ConstraintPtr c, cp_) {
      if (ite == result.end()) break;
      if (c->is_soft()) {
        if (*ite == cnt) {
          ite++;
          inactive_softs_.push_back(c->get_name());
        }
        cnt++;
      }
    }
  }

};

struct VariableCoverageSolver : VariableSolver {

  VariableCoverageSolver(VariableContainer& vcon, ConstraintPartition& cp)
  : VariableSolver(vcon, cp) {

    LOG(INFO) << "Create coverage solver for partition " << cp_;

    BOOST_FOREACH(ConstraintPtr c, cp_) {
      if (c->is_soft()) continue; // coverage solver ignores soft constraints for now
      if (!c->is_cover())
        solver_->makeAssertion(*c->get_expression());
    }
  }

  virtual bool solve() {
    BOOST_FOREACH(ConstraintPtr c, cp_) {
      if (!c->is_cover()) continue;
      if (covered_.find(c->get_name()) != covered_.end()) continue; // alread covered
      // try solve
      BOOST_FOREACH(VariableContainer::ReadRefPair pair, vcon_.read_references)
        if (cp_.contains_var(pair.first))
          solver_->makeAssumption(*pair.second->expr());
      solver_->makeAssumption(*c->get_expression());
      if (solver_->solve()) { 
        LOG(INFO) << "Solve partition " << cp_ << " hitting constraint " << c->get_name();
        covered_.insert(c->get_name());
        BOOST_FOREACH(VariableContainer::WriteRefPair pair, vcon_.write_references)
          if (cp_.contains_var(pair.first))
            solver_->read(*vcon_.variables[pair.first], *pair.second);        
        return true;
      }
    }
    return false;
  }

private:
  std::set<std::string> covered_;
};

/**
 *
 */
struct VariableGenerator {
  typedef boost::shared_ptr<VariableSolver> VarSolverPtr;

  VariableGenerator(VariableContainer& vcon) 
  : vcon_(vcon), solvers_() { }

  virtual void reset(std::vector<ConstraintPartition>& partitions) {
    solvers_.clear();
    BOOST_FOREACH(ConstraintPartition& partition, partitions) {
      VarSolverPtr vs(new VariableDefaultSolver(vcon_, partition));
      solvers_.push_back(vs);
    }
  }

  virtual bool solve() {
    BOOST_FOREACH(VarSolverPtr vs, solvers_)
      if (!vs->solve()) return false;
    return true;
  }

  template<typename T>
  bool read(Variable<T> const &var, T& value) {
    BOOST_FOREACH(VarSolverPtr vs, solvers_)
      if (vs->read(var, value)) return true;
    return false;
  }

  std::vector<std::vector<std::string> > analyse_contradiction() {
    std::vector<std::vector<std::string> > str_vec;
    BOOST_FOREACH(VarSolverPtr vs, solvers_) {
      std::vector<std::vector<std::string> > c = vs->analyse_contradiction();
      if (!c.empty())
        str_vec.insert(str_vec.end(), c.begin(), c.end());
    }
    return str_vec;
  }

  std::vector<std::string> get_inactive_softs() {
    std::vector<std::string> str_vec;
    BOOST_FOREACH(VarSolverPtr vs, solvers_) {
      std::vector<std::string> c = vs->get_inactive_softs();
      if (!c.empty())
        str_vec.insert(str_vec.end(), c.begin(), c.end());
    }
    return str_vec;
  }

protected:
  VariableContainer& vcon_;
  std::vector<VarSolverPtr> solvers_;
};

struct VariableCoverageGenerator : VariableGenerator {
 
  VariableCoverageGenerator(VariableContainer& vcon) 
  : VariableGenerator(vcon) { }

  virtual void reset(std::vector<ConstraintPartition>& partitions) {
    solvers_.clear();
    BOOST_FOREACH(ConstraintPartition& partition, partitions) {
      VarSolverPtr vs(new VariableCoverageSolver(vcon_, partition));
      solvers_.push_back(vs);
    }
  }

  virtual bool solve() {
    bool res = false;
    BOOST_FOREACH(VarSolverPtr vs, solvers_)
      if (vs->solve()) res = true; // one solver has hitted an uncovered constraint -> everything ok, from covered solvers, the last solutions will be reused.  
    return res;
  }
};


} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

