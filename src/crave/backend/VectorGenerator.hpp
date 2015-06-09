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

  VectorSolver(int vector_id, const VariableGenerator& var_gen);

  void addConstraint(VectorConstraintPtr vc);

  bool solve();

 private:
  void resetSolver(unsigned int const size);

  void buildSolver(unsigned int const size);

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

  explicit VectorGenerator(const VariableGenerator& var_gen);

  bool solve();

  void reset(const std::vector<VectorConstraintPtr>& v);

 private:
  void addConstraint(VectorConstraintPtr vc);

 private:
  VectorSolverMap vector_solvers_;
  const VariableGenerator& var_gen_;
};

}  // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab
