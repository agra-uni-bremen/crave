// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/scoped_ptr.hpp>
#include <vector>
#include <map>
#include <string>

#include "VariableSolver.hpp"

namespace crave {

struct VariableDefaultSolver : VariableSolver {
  VariableDefaultSolver(const VariableContainer& vcon,
                        const ConstraintPartition& cp)
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
      BOOST_FOREACH(std::string& s, inactive_softs_) { LOG(INFO) << " " << s; }
    } else {
      LOG(INFO) << "Partition has unsatisfiable hard constraints:";
      uint cnt = 0;

      BOOST_FOREACH(std::vector<std::string>& vs, contradictions_) {
        LOG(INFO) << "  set #" << ++cnt;
        BOOST_FOREACH(std::string& s, vs) { LOG(INFO) << "   " << s; }
      }
    }
  }

  virtual bool solve() {
    if (!contradictions_.empty()) return false;
    BOOST_FOREACH(VariableContainer::ReadRefPair pair,
                   var_ctn.read_references) {
      if (constr_pttn.containsVar(pair.first)) {
        solver->makeAssumption(*pair.second->expr());
      }
    }
    BOOST_FOREACH(VariableContainer::ReadRefPair pair,
                   var_ctn.dist_references) {
      if (constr_pttn.containsVar(pair.first)) {
        solver->makeSuggestion(*pair.second->expr());
      }
    }
    if (solver->solve()) {
      BOOST_FOREACH(VariableContainer::WriteRefPair pair,
                     var_ctn.write_references) {
        if (constr_pttn.containsVar(pair.first)) {
          solver->read(*var_ctn.variables[pair.first], *pair.second);
        }
      }
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

    BOOST_FOREACH(ConstraintPtr c, constr_pttn) {
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
}  // namespace crave
