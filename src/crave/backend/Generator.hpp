#pragma once

#include "../ir/Context.hpp"
#include "../ir/UserConstraint.hpp"
#include "../ir/VariableContainer.hpp"
#include "../backend/VectorGenerator.hpp"

#include <boost/foreach.hpp>
#include <boost/function.hpp>

#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace crave {

struct Generator {

public:
  Generator()
  : constr_mng(), var_ctn(crave::variables), ctx(var_ctn)
  , var_gen(var_ctn)
  , vec_gen(var_gen)
  , var_cov_gen(var_ctn)
  , vec_cov_gen(var_cov_gen)
  , covered_(false) {
  }

  template<typename Expr>
  Generator(Expr expr)
  : constr_mng(), var_ctn(crave::variables), ctx(var_ctn) 
  , var_gen(var_ctn)
  , vec_gen(var_gen)
  , var_cov_gen(var_ctn)
  , vec_cov_gen(var_cov_gen)
  , covered_(false) {
    (*this)(expr);
  }

  template<typename Expr>
  Generator & operator()(Expr expr) {
    constr_mng.makeConstraint(expr, ctx);
    return *this;
  }

  template<typename Expr>
  Generator & operator()(std::string constraint_name, Expr expr) {
    constr_mng.makeConstraint(constraint_name, expr, ctx);
    return *this;
  }

  template<typename Expr>
  Generator & soft(Expr e) {
    constr_mng.makeConstraint(e, ctx, true);
    return *this;
  }

  template<typename Expr>
  Generator & soft(std::string name, Expr e) {
    constr_mng.makeConstraint(name, e, ctx, true);
    return *this;
  }

  template<typename Expr>
  Generator & cover(Expr e) {
    constr_mng.makeConstraint(e, ctx, false, true);
    return *this;
  }

  template<typename Expr>
  Generator & cover(std::string name, Expr e) {
    constr_mng.makeConstraint(name, e, ctx, false, true);
    return *this;
  }

  bool enableConstraint(std::string const& name) { return constr_mng.enable_constraint(name); }
  bool disableConstraint(std::string const& name) { return constr_mng.disable_constraint(name); }
  bool isConstraintEnabled(std::string const& name) { return constr_mng.is_constraint_enabled(name); }
  bool isChanged() { return constr_mng.is_changed(); }

  Generator & operator()() {
    if (!next())
      throw std::runtime_error("Generator constraint unsatisfiable.");
    return *this;
  }

  void merge(Generator& other) { constr_pttn.mergeConstraints(other.constr_mng); }

  void reset() {
    constr_mng.set_synced();
    constr_pttn.reset();
    resetCoverage();
  }

  void rebuild(bool selfInclude = false) {
    if (selfInclude) merge(*this);
    constr_pttn.partition();
    // currently, every hard/soft/cover constraint is considered for partitioning, this is suboptimal.
    var_gen.reset(constr_pttn.get_partitions());
    vec_gen.reset(constr_pttn.get_vector_constraints());
    var_cov_gen.reset(constr_pttn.get_partitions()); 
    vec_cov_gen.reset(constr_pttn.get_vector_constraints());
  }

  bool next() {
    if (constr_mng.is_changed()) {
      reset();
      rebuild(true);
    }
    return var_gen.solve() && vec_gen.solve();
  }

  bool nextCov() {
    if (constr_mng.is_changed()) {
      reset();
      rebuild(true);
    }
    if (!covered_) {
      if (var_cov_gen.solve() && vec_cov_gen.solve())
        return true;
      else
        covered_ = true;
    }
    return next();
  }

  bool isCovered() { return covered_; }
  
  void resetCoverage() { 
    covered_ = false; 
    var_cov_gen.reset(constr_pttn.get_partitions());
    vec_cov_gen.reset(constr_pttn.get_vector_constraints());
  }

  std::ostream& printDotGraph(std::ostream& os, bool root = true) { 
    if (root)
      os << "digraph AST {" << std::endl;
    constr_mng.print_dot_graph_(os);
    if (root)
      os << "}" << std::endl;
    return os;
  }

  std::vector<std::vector<std::string> > analyseContradiction() {
    return var_gen.analyseContradiction();
  }

  std::vector<std::string> getInactiveSofts() {
    return var_gen.getInactiveSofts();
  }

  template<typename T>
  T operator[](Variable<T> const &var) {
    T result;
    if (!var_gen.read(var, result)) {
      throw std::runtime_error("Invalid variable read request.");
    }
    return result;
  }  

private:
  // constraints
  ConstraintManager constr_mng;
  ConstraintPartitioner constr_pttn;

  // variables
  VariableContainer& var_ctn;

  // context
  Context ctx;

  // solvers
  VariableGenerator var_gen;
  VectorGenerator vec_gen;

  VariableCoverageGenerator var_cov_gen;
  VectorGenerator vec_cov_gen;

  // coverage
  bool covered_;
};

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

