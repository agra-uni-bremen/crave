// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <vector>
#include <map>

#include "VariableGenerator.hpp"
#include "../ir/visitor/ReplaceVisitor.hpp"

namespace crave {

/*
 *
 */
struct VectorSolver {
  typedef boost::intrusive_ptr<VariableExpr> VariablePtr;
  typedef std::vector<VariablePtr> VectorElements;

  VectorSolver(int vector_id_, const VariableGenerator& var_gen_)
      : constraints_(),
        vector_id_(vector_id_),
        solver_(FactoryMetaSMT::getNewInstance()),
        vec_elements_(),
        var_gen_(var_gen_) {}

  void addConstraint(VectorConstraintPtr vc) { constraints_.push_back(vc); }

  bool solve() {
    __rand_vec_base* vector = vectorBaseMap[vector_id_];

    unsigned int size = default_rand_vec_size();
    if (!var_gen_.read(vector->size_var(), &size)) {
      LOG(INFO) << "Use default size for vector " << vector_id_;
    }
    resetSolver(size);
    bool result = solver_->solve(false) || solver_->solve(true);
    if (result) solver_->readVector(vec_elements_, vector);

    return result;
  }

 private:
  void resetSolver(unsigned int const size) {
    solver_.reset(FactoryMetaSMT::getNewInstance());
    buildSolver(size);
  }

  void buildSolver(unsigned int const size) {
    if (vec_elements_.size() != size) {
      unsigned int old_size = vec_elements_.size();
      vec_elements_.resize(size);
      for (unsigned int i = old_size; i < size; ++i) {
        vec_elements_[i] = new VariableExpr(new_var_id(), 1u, true);
      }
    }

    BOOST_FOREACH(VectorConstraintPtr constraint, constraints_) {
      if (!constraint->isUnique()) {
        ReplaceVisitor replacer(&vec_elements_);
        for (unsigned int i = 0u; i < size; ++i) {
          replacer.setVecIdx(i);
          constraint->expr()->visit(&replacer);

          if (replacer.okay()) {
            if (constraint->isSoft())
              solver_->makeSoftAssertion(*replacer.result());
            else
              solver_->makeAssertion(*replacer.result());
          }

          replacer.reset();
        }
      } else {
        for (uint i = 0; i < vec_elements_.size(); i++)
          for (uint j = i + 1; j < vec_elements_.size(); ++j) {
            NotEqualOpr neOp(vec_elements_[i], vec_elements_[j]);
            if (constraint->isSoft())
              solver_->makeSoftAssertion(neOp);
            else
              solver_->makeAssertion(neOp);
          }
      }
    }
  }

 private:
  std::vector<VectorConstraintPtr> constraints_;
  int vector_id_;
  SolverPtr solver_;
  VectorElements vec_elements_;

  const VariableGenerator& var_gen_;
};

/*
 *
 */
struct VectorGenerator {
  typedef std::map<int, VectorSolver> VectorSolverMap;

  explicit VectorGenerator(const VariableGenerator& var_gen_)
      : vector_solvers_(), var_gen_(var_gen_) {}

  bool solve() {
    BOOST_FOREACH(VectorSolverMap::value_type& c_pair, vector_solvers_) {
      if (!c_pair.second.solve()) return false;
    }
    return true;
  }

  void reset(const std::vector<VectorConstraintPtr>& v) {
    vector_solvers_.clear();
    BOOST_FOREACH(VectorConstraintPtr vc, v) { addConstraint(vc); }
  }

 private:
  void addConstraint(VectorConstraintPtr vc) {
    int v_id = vc->getVectorId();
    VectorSolverMap::iterator ite(vector_solvers_.lower_bound(v_id));
    if (ite != vector_solvers_.end() &&
        !(vector_solvers_.key_comp()(v_id, ite->first))) {
      ite->second.addConstraint(vc);
    } else {
      VectorSolver vs(v_id, var_gen_);
      vs.addConstraint(vc);
      vector_solvers_.insert(std::make_pair(v_id, vs));
    }
  }

 private: 
  VectorSolverMap vector_solvers_;
  const VariableGenerator& var_gen_;
};

}  // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab
