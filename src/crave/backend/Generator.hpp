// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <string>
#include <vector>
#include "../ir/UserConstraint.hpp"
#include "VariableGenerator.hpp"
#include "../backend/VectorGenerator.hpp"

namespace crave {

extern VariableContainer variables;

struct Generator {
 public:
  Generator();

  template <typename Expr>
  explicit Generator(Expr expr)
      : constr_mng_(),
        var_ctn_(&variables),
        ctx_(var_ctn_),
        var_gen_(var_ctn_),
        vec_gen_(var_gen_),
        var_cov_gen_(var_ctn_),
        vec_cov_gen_(var_cov_gen_),
        covered_(false) {
    (*this)(expr);
  }

  template <typename Expr>
  Generator& operator()(Expr expr) {
    constr_mng_.makeConstraint(expr, &ctx_);
    return *this;
  }

  template <typename Expr>
  Generator& operator()(std::string constraint_name, Expr expr) {
    constr_mng_.makeConstraint(constraint_name, expr, &ctx_);
    return *this;
  }

  template <typename Expr>
  Generator& soft(Expr e) {
    constr_mng_.makeConstraint(e, &ctx_, true);
    return *this;
  }

  template <typename Expr>
  Generator& soft(std::string name, Expr e) {
    constr_mng_.makeConstraint(name, e, &ctx_, true);
    return *this;
  }

  template <typename Expr>
  Generator& cover(Expr e) {
    constr_mng_.makeConstraint(e, &ctx_, false, true);
    return *this;
  }

  template <typename Expr>
  Generator& cover(std::string name, Expr e) {
    constr_mng_.makeConstraint(name, e, &ctx_, false, true);
    return *this;
  }

  bool enableConstraint(std::string const& name);

  bool disableConstraint(std::string const& name);

  bool isConstraintEnabled(std::string const& name);

  bool isChanged();

  Generator& operator()();

  void merge(const Generator& other);

  void reset();

  void rebuild(bool selfInclude = false);

  bool next();

  bool nextCov();

  bool isCovered();

  void resetCoverage();

  std::ostream& printDotGraph(std::ostream& os, bool root = true);

  std::vector<std::vector<std::string> > analyseContradiction();

  std::vector<std::string> getInactiveSofts();

  template <typename T>
  T operator[](Variable<T> const& var) {
    T result;
    if (!var_gen_.read(var, &result)) {
      throw std::runtime_error("Invalid variable read request.");
    }
    return result;
  }

 private:
  // constraints
  ConstraintManager constr_mng_;
  ConstraintPartitioner constr_pttn_;

  // variables
  VariableContainer* var_ctn_;

  // context
  Context ctx_;

  // solvers
  VariableGenerator var_gen_;
  VectorGenerator vec_gen_;

  VariableCoverageGenerator var_cov_gen_;
  VectorGenerator vec_cov_gen_;

  // coverage
  bool covered_;
};

}  // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab
