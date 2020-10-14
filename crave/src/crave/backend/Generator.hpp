/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/


#pragma once

#include <string>
#include <vector>
#include "../ir/UserConstraint.hpp"
#include "VariableGenerator.hpp"
#include "../backend/VectorGenerator.hpp"

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
