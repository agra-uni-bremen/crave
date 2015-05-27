// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <string>
#include <vector>
#include "../ir/UserConstraint.hpp"
#include "VariableGenerator.hpp"
#include "../backend/VectorGenerator.hpp"

namespace crave {

struct Generator {
 public:
  Generator()
      : constr_mng_(),
        var_ctn_(&crave::variables),
        ctx_(var_ctn_),
        var_gen_(var_ctn_),
        vec_gen_(var_gen_),
        var_cov_gen_(var_ctn_),
        vec_cov_gen_(var_cov_gen_),
        covered_(false) {}

  template <typename Expr>
  explicit Generator(Expr expr)
      : constr_mng_(),
        var_ctn_(&crave::variables),
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
    constr_mng_.makeConstraint(expr, ctx_);
    return *this;
  }

  template <typename Expr>
  Generator& operator()(std::string constraint_name, Expr expr) {
    constr_mng_.makeConstraint(constraint_name, expr, ctx_);
    return *this;
  }

  template <typename Expr>
  Generator& soft(Expr e) {
    constr_mng_.makeConstraint(e, ctx_, true);
    return *this;
  }

  template <typename Expr>
  Generator& soft(std::string name, Expr e) {
    constr_mng_.makeConstraint(name, e, ctx_, true);
    return *this;
  }

  template <typename Expr>
  Generator& cover(Expr e) {
    constr_mng_.makeConstraint(e, ctx_, false, true);
    return *this;
  }

  template <typename Expr>
  Generator& cover(std::string name, Expr e) {
    constr_mng_.makeConstraint(name, e, ctx_, false, true);
    return *this;
  }

  bool enableConstraint(std::string const& name) {
    return constr_mng_.enableConstraint(name);
  }

  bool disableConstraint(std::string const& name) {
    return constr_mng_.disableConstraint(name);
  }

  bool isConstraintEnabled(std::string const& name) {
    return constr_mng_.isConstraintEnabled(name);
  }

  bool isChanged() { return constr_mng_.isChanged(); }

  Generator& operator()() {
    if (!next()) {
      throw std::runtime_error("Generator constraint unsatisfiable.");
    }
    return *this;
  }

  void merge(const Generator& other) {
    constr_pttn_.mergeConstraints(other.constr_mng_);
  }

  void reset() {
    constr_mng_.resetChanged();
    constr_pttn_.reset();
    resetCoverage();
  }

  void rebuild(bool selfInclude = false) {
    if (selfInclude) merge(*this);
    constr_pttn_.partition();
    // currently, every hard/soft/cover constraint is considered for
    // partitioning, this is suboptimal.
    var_gen_.reset(constr_pttn_.getPartitions());
    vec_gen_.reset(constr_pttn_.getVectorConstraints());
    var_cov_gen_.reset(constr_pttn_.getPartitions());
    vec_cov_gen_.reset(constr_pttn_.getVectorConstraints());
  }

  bool next() {
    if (constr_mng_.isChanged()) {
      reset();
      rebuild(true);
    }
    return var_gen_.solve() && vec_gen_.solve();
  }

  bool nextCov() {
    if (constr_mng_.isChanged()) {
      reset();
      rebuild(true);
    }
    if (!covered_) {
      if (var_cov_gen_.solve() && vec_cov_gen_.solve())
        return true;
      else
        covered_ = true;
    }
    return next();
  }

  bool isCovered() { return covered_; }

  void resetCoverage() {
    covered_ = false;
    var_cov_gen_.reset(constr_pttn_.getPartitions());
    vec_cov_gen_.reset(constr_pttn_.getVectorConstraints());
  }

  std::ostream& printDotGraph(std::ostream& os, bool root = true) {
    if (root) os << "digraph AST {" << std::endl;
    constr_mng_.printDotGraph(os);
    if (root) os << "}" << std::endl;
    return os;
  }

  std::vector<std::vector<std::string> > analyseContradiction() {
    return var_gen_.analyseContradiction();
  }

  std::vector<std::string> getInactiveSofts() {
    return var_gen_.getInactiveSofts();
  }

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
