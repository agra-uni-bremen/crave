#pragma once

#include "Context.hpp"
#include "UserConstraint.hpp"
#include "VariableContainer.hpp"
#include "VectorGenerator.hpp"
#include "expression/Node.hpp"

#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

namespace crave {

struct Generator {

public:
  Generator()
  : pre_hooks_(), constr_mng_(), vcon_(crave::variables), ctx_(vcon_)
  , var_gen_(vcon_)
  , vec_gen_(var_gen_) {
  }

  template<typename Expr>
  Generator(Expr expr)
  : pre_hooks_(), constr_mng_(), vcon_(crave::variables), ctx_(vcon_) 
  , var_gen_(vcon_)
  , vec_gen_(var_gen_) {
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

  inline bool enable_constraint(std::string const& name) { return constr_mng_.enable_constraint(name); }
  inline bool disable_constraint(std::string const& name) { return constr_mng_.disable_constraint(name); }
  inline bool is_constraint_enabled(std::string const& name) { return constr_mng_.is_constraint_enabled(name); }

  void add_pre_hook(boost::function0<bool> f) {
    pre_hooks_.push_back(f);
  }

  Generator & operator()() {
    if (!next())
      throw std::runtime_error("Generator constraint unsatisfiable.");
    return *this;
  }

  void reset() {
    constr_mng_.partition();
    var_gen_.reset(constr_mng_.get_partitions());
    vec_gen_.reset(constr_mng_.get_vector_constraints());
  }

  bool next() {
    BOOST_FOREACH(boost::function0<bool> f, pre_hooks_)
      if (!f())
        return false;
    if (constr_mng_.is_changed()) {
      reset();
      constr_mng_.set_synced();
    }
    return var_gen_.solve() && vec_gen_.solve();
  }

  inline std::ostream& print_dot_graph(std::ostream& os) { 
    return constr_mng_.print_dot_graph(os);  
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
  std::vector<boost::function0<bool> > pre_hooks_;

  // constraints
  ConstraintManager constr_mng_;

  // variables
  VariableContainer& vcon_;

  // context
  Context ctx_;

  // solvers
  VariableGenerator var_gen_;
  VectorGenerator vec_gen_;
};

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

