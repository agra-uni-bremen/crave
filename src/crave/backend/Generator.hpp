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
  : constr_mng_(), vcon_(crave::variables), ctx_(vcon_)
  , var_gen_(vcon_)
  , vec_gen_(var_gen_)
  , var_cov_gen_(vcon_)
  , vec_cov_gen_(var_cov_gen_)
  , covered_(false) {
  }

  template<typename Expr>
  Generator(Expr expr)
  : constr_mng_(), vcon_(crave::variables), ctx_(vcon_) 
  , var_gen_(vcon_)
  , vec_gen_(var_gen_)
  , var_cov_gen_(vcon_)
  , vec_cov_gen_(var_cov_gen_)
  , covered_(false) {
    (*this)(expr);
  }

  template<typename Expr>
  Generator & operator()(Expr expr) {
    constr_mng_.makeConstraint(expr, ctx_);
    return *this;
  }

  template<typename Expr>
  Generator & operator()(std::string constraint_name, Expr expr) {
    constr_mng_.makeConstraint(constraint_name, expr, ctx_);
    return *this;
  }

  template<typename Expr>
  Generator & soft(Expr e) {
    constr_mng_.makeConstraint(e, ctx_, true);
    return *this;
  }

  template<typename Expr>
  Generator & soft(std::string name, Expr e) {
    constr_mng_.makeConstraint(name, e, ctx_, true);
    return *this;
  }

  template<typename Expr>
  Generator & cover(Expr e) {
    constr_mng_.makeConstraint(e, ctx_, false, true);
    return *this;
  }

  template<typename Expr>
  Generator & cover(std::string name, Expr e) {
    constr_mng_.makeConstraint(name, e, ctx_, false, true);
    return *this;
  }

  inline bool enable_constraint(std::string const& name) { return constr_mng_.enable_constraint(name); }
  inline bool disable_constraint(std::string const& name) { return constr_mng_.disable_constraint(name); }
  inline bool is_constraint_enabled(std::string const& name) { return constr_mng_.is_constraint_enabled(name); }
  inline bool is_changed() { return constr_mng_.is_changed(); }

  Generator & operator()() {
    if (!next())
      throw std::runtime_error("Generator constraint unsatisfiable.");
    return *this;
  }

  inline void merge(Generator& other) { cp.mergeConstraints(other.constr_mng_); }

  void reset() {
    constr_mng_.set_synced();
    cp.reset();
    reset_coverage();
  }

  void rebuild(bool selfInclude = false) {
    if (selfInclude) merge(*this);
    cp.partition();
    // currently, every hard/soft/cover constraint is considered for partitioning, this is suboptimal.
    var_gen_.reset(cp.get_partitions());
    vec_gen_.reset(cp.get_vector_constraints());
    var_cov_gen_.reset(cp.get_partitions()); 
    vec_cov_gen_.reset(cp.get_vector_constraints());
  }

  bool next() {
    if (constr_mng_.is_changed()) {
      reset();
      rebuild(true);
    }
    return var_gen_.solve() && vec_gen_.solve();
  }

  bool next_cov() {
    if (constr_mng_.is_changed()) {
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

  bool is_covered() { return covered_; }
  void reset_coverage() { 
    covered_ = false; 
    var_cov_gen_.reset(cp.get_partitions());
    vec_cov_gen_.reset(cp.get_vector_constraints());
  }

  inline std::ostream& print_dot_graph(std::ostream& os, bool root = true) { 
    if (root)
      os << "digraph AST {" << std::endl;
    constr_mng_.print_dot_graph_(os);
    if (root)
      os << "}" << std::endl;
    return os;
  }

  inline std::vector<std::vector<std::string> > analyse_contradiction() {
    return var_gen_.analyse_contradiction();
  }

  inline std::vector<std::string> get_inactive_softs() {
    return var_gen_.get_inactive_softs();
  }

  template<typename T>
  T operator[](Variable<T> const &var) {
    T result;
    if (!var_gen_.read(var, result)) {
      throw std::runtime_error("Invalid variable read request.");
    }
    return result;
  }  

private:
  // constraints
  ConstraintManager constr_mng_;
  ConstraintPartitioner cp;

  // variables
  VariableContainer& vcon_;

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

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

