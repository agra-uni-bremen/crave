// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <string>
#include <vector>

#include "../backend/VectorGenerator.hpp"
#include "../ir/UserConstraint.hpp"
#include "VariableGenerator.hpp"

namespace crave {

/*!
 * \brief Backend generator to generate random values with constraints.
 *
 * Try to avoid usage of this class directly.
 * CRAVE usually uses this class internally.
 */
struct Generator {
 public:
  Generator()
      : constr_mng_(),
        var_ctn_(variable_container()),
        ctx_(var_ctn_),
        var_gen_(new VariableGenerator(*var_ctn_)),
        var_cov_gen_(*var_ctn_),
        vec_gen_(),
        covered_(false) {}

  template <typename Expr>
  explicit Generator(Expr expr)
      : constr_mng_(),
        var_ctn_(variable_container()),
        ctx_(var_ctn_),
        var_gen_(new VariableGenerator(*var_ctn_)),
        var_cov_gen_(*var_ctn_),
        vec_gen_(),
        covered_(false) {
    (*this)(expr);
  }

  virtual ~Generator() { delete var_gen_; }

  void enable_multithreading();

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
    if (!var_gen_->read(var, &result)) {
      throw std::runtime_error("Invalid variable read request.");
    }
    return result;
  }

  void addVecId(int id) { to_be_generated_vec_ids_.insert(id); }

 private:
  // constraints
  ConstraintManager constr_mng_;
  ConstraintPartitioner constr_pttn_;

  // variables
  VariableContainer* var_ctn_;
  std::set<int> to_be_generated_vec_ids_;

  // context
  Context ctx_;

  // variable solvers
  VariableGenerator* var_gen_;
  VariableCoverageGenerator var_cov_gen_;

  // vector solver
  VectorGenerator vec_gen_;

  // coverage
  bool covered_;
};

}  // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab
