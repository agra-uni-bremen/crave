// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <map>
#include <vector>

#include "../ir/visitor/ReplaceVisitor.hpp"
#include "VariableGenerator.hpp"

namespace crave {

/*
 *
 */
struct VectorSolver {
  typedef std::shared_ptr<VariableExpr> VariablePtr;
  typedef std::vector<VariablePtr> VectorElements;

  explicit VectorSolver(int vector_id);

  void addConstraint(VectorConstraintPtr vc);
  bool solve(const VariableGenerator& var_gen);

 private:
  void resetSolver(unsigned int const size);
  void buildSolver(unsigned int const size);

 private:
  std::vector<VectorConstraintPtr> constraints_;
  int vector_id_;
  SolverPtr solver_;
  VectorElements vec_elements_;
};

/*
 *
 */
struct VectorGenerator {
  typedef std::map<int, VectorSolver> VectorSolverMap;

  VectorGenerator();
  bool solve(const VariableGenerator& var_gen, const std::set<int>& vec_ids);
  void reset(const std::vector<VectorConstraintPtr>& v);

 private:
  void addConstraint(VectorConstraintPtr vc);

 private:
  VectorSolverMap vector_solvers_;
};

}  // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab
