// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include "VariableGenerator.hpp"
#include "../ir/visitor/ReplaceVisitor.hpp"

namespace crave {

/*
 *
 */
struct VectorSolver {

  typedef boost::intrusive_ptr<VariableExpr> VariablePtr;
  typedef std::vector<VariablePtr> VectorElements;

  VectorSolver(int vector_id_, VariableGenerator& var_gen_)
      : constraints(),
        vector_id(vector_id_),
        solver(FactoryMetaSMT::getNewInstance()),
        vec_elements(),
        var_gen(var_gen_) {}

  void addConstraint(VectorConstraintPtr vc) { constraints.push_back(vc); }

  bool solve() {
    __rand_vec_base* vector = vectorBaseMap[vector_id];

    unsigned int size = default_rand_vec_size();
    if (!var_gen.read(vector->size_var(), size)) LOG(INFO) << "Use default size for vector " << vector_id;
    resetSolver(size);
    bool result = solver->solve(false) || solver->solve(true);
    if (result) solver->readVector(vec_elements, *vector);

    return result;
  }

 private:
  void resetSolver(unsigned int const size) {
    solver.reset(FactoryMetaSMT::getNewInstance());
    buildSolver(size);
  }

  void buildSolver(unsigned int const size) {

    if (vec_elements.size() != size) {
      unsigned int old_size = vec_elements.size();
      vec_elements.resize(size);
      for (unsigned int i = old_size; i < size; ++i) vec_elements[i] = new VariableExpr(new_var_id(), 1u, true);
    }

    BOOST_FOREACH(VectorConstraintPtr constraint, constraints) {

      if (!constraint->isUnique()) {

        ReplaceVisitor replacer(vec_elements);
        for (unsigned int i = 0u; i < size; ++i) {

          replacer.setVecIdx(i);
          constraint->expr()->visit(replacer);

          if (replacer.okay()) {
            if (constraint->isSoft())
              solver->makeSoftAssertion(*replacer.result());
            else
              solver->makeAssertion(*replacer.result());
          }

          replacer.reset();
        }
      } else {
        for (uint i = 0; i < vec_elements.size(); i++)
          for (uint j = i + 1; j < vec_elements.size(); ++j) {
            NotEqualOpr neOp(vec_elements[i], vec_elements[j]);
            if (constraint->isSoft())
              solver->makeSoftAssertion(neOp);
            else
              solver->makeAssertion(neOp);
          }
      }
    }
  }

 private:
  std::vector<VectorConstraintPtr> constraints;
  int vector_id;
  SolverPtr solver;
  VectorElements vec_elements;

  VariableGenerator& var_gen;
};

/*
 *
 */
struct VectorGenerator {

  typedef std::map<int, VectorSolver> VectorSolverMap;

  VectorGenerator(VariableGenerator& var_gen_) : vector_solvers(), var_gen(var_gen_) {}

  bool solve() {
    BOOST_FOREACH(VectorSolverMap::value_type & c_pair, vector_solvers) {
      if (!c_pair.second.solve()) return false;
    }
    return true;
  }

  void reset(std::vector<VectorConstraintPtr>& v) {
    vector_solvers.clear();
    BOOST_FOREACH(VectorConstraintPtr vc, v) { addConstraint(vc); }
  }

 private:
  void addConstraint(VectorConstraintPtr vc) {
    int v_id = vc->getVectorId();
    VectorSolverMap::iterator ite(vector_solvers.lower_bound(v_id));
    if (ite != vector_solvers.end() && !(vector_solvers.key_comp()(v_id, ite->first))) {
      ite->second.addConstraint(vc);
    } else {
      VectorSolver vs(v_id, var_gen);
      vs.addConstraint(vc);
      vector_solvers.insert(std::make_pair(v_id, vs));
    }
  }

 private:
  VectorSolverMap vector_solvers;
  VariableGenerator& var_gen;
};

}  // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab
