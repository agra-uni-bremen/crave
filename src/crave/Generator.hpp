#pragma once

#include "Context.hpp"
#include "UserConstraint.hpp"
#include "VariableContainer.hpp"
#include "VectorGenerator.hpp"
#include "expression/ToDotNodeVisitor.hpp"
#include "expression/FactoryMetaSMT.hpp"
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

struct Generator;
typedef Generator DefaultGenerator;

struct Generator {

public:
  Generator()
  : constraints_(), vcon_(crave::variables), pre_hooks_(),
    ctx_(vcon_), solver_(FactoryMetaSMT::getNewInstance()), exact_analyse_(false), vector_gen_(vcon_, solver_, exact_analyse_) {
  }

  template<typename Expr>
  Generator(Expr expr)
  : constraints_(), vcon_(crave::variables), pre_hooks_(),
    ctx_(vcon_), solver_(FactoryMetaSMT::getNewInstance()), exact_analyse_(false), vector_gen_(vcon_, solver_, exact_analyse_) {
      (*this)(expr);
    }

  template<typename Expr>
  Generator & operator()(Expr expr) {
    constraints_.push_back(constraintManager.makeConstraint(expr, ctx_));
    return *this;
  }

  template<typename Expr>
  Generator & operator()(std::string constraint_name, Expr expr) {
    constraints_.push_back(constraintManager.makeConstraint(constraint_name, expr, ctx_));
    return *this;
  }

private:
  void build_solver_() {
    BOOST_FOREACH (ConstraintPtr c, constraints_) {
      if (c->is_enabled()) {
        if (c->is_soft()) {
          solver_->makeSoftAssertion(*c->get_expression());
        } else {
          solver_->makeAssertion(*c->get_expression());
        }
      }
    }
  }

public:
  void reset() {
    solver_.reset(FactoryMetaSMT::getNewInstance());
    build_solver_();
  }

  std::vector<std::vector<std::string> > analyse_contradiction() {

    boost::scoped_ptr<metaSMTVisitor> solver(FactoryMetaSMT::getNewInstance());
    std::vector<std::vector<std::string> > str_vec;

    std::map<unsigned int, NodePtr> s;
    std::vector<std::string> out;
    std::vector<std::vector<unsigned int> > results;

    BOOST_FOREACH(ConstraintPtr c, constraints_)
    {
      s.insert(std::make_pair(s.size(), c->get_expression()));
      out.push_back(c->get_name());
    }

    results = solver->analyseContradiction(s);

    BOOST_FOREACH(std::vector<unsigned int> result, results)
    {
      std::vector<std::string> vec;
      BOOST_FOREACH(unsigned int i, result)
      {
        vec.push_back(out[i]);
      }
      str_vec.push_back(vec);
    }
    return str_vec;
  }

  bool enable_constraint(std::string const& name) {
    return constraints_.enable_constraint(name) || vector_gen_.enable_constraint(name);
  }

  bool disable_constraint(std::string const& name) {
    return constraints_.disable_constraint(name) || vector_gen_.disable_constraint(name);
  }

  bool is_constraint_enabled(std::string const& name) {
    return constraints_.is_constraint_enabled(name) || vector_gen_.is_constraint_enabled(name);
  }

  void add_pre_hook(boost::function0<bool> f) {
    pre_hooks_.push_back(f);
  }

  /**
   * generate a new assignment
   **/
  Generator & operator()() {
    if (!next())
      throw std::runtime_error("Generator constraint unsatisfiable.");
    return *this;
  }

  template<typename Expr>
  Generator & soft(Expr e) {
    constraints_.push_back(constraintManager.makeConstraint(e, ctx_, true));
    return *this;
  }

  template<typename Expr>
  Generator & soft(std::string name, Expr e) {
    constraints_.push_back(constraintManager.makeConstraint(name, e, ctx_, true));
    return *this;
  }

  /**
   * foreach
   **/
  template<typename value_type, typename Expr>
  Generator & foreach(__rand_vec <value_type> & v,
                      const placeholder & p, Expr e) {
    vector_gen_.addForEach(constraintManager.makeConstraint(e, ctx_), v);
    return *this;
  }

  template<typename value_type, typename Expr>
  Generator & soft_foreach(__rand_vec <value_type> & v,
                           const placeholder & p, Expr e) {
    vector_gen_.addForEach(constraintManager.makeConstraint(e, ctx_, true), v);
    return *this;
  }

  template<typename value_type, typename Expr>
  Generator & foreach(std::string constraint_name,
      __rand_vec <value_type> & v, const placeholder & p, Expr e) {
    vector_gen_.addForEach(constraintManager.makeConstraint(constraint_name, e, ctx_), v);
    return *this;
  }

  /**
   * unique & non_unique
   **/
  template<typename value_type>
  Generator & unique(const __rand_vec <value_type> & v) {
    vector_gen_.setUnique(v, true);
    return *this;
  }

  template<typename value_type>
  Generator & non_unique(const __rand_vec <value_type> & v) {
    vector_gen_.setUnique(v, false);
    return *this;
  }

private:
  bool pre_solve_() {
    BOOST_FOREACH(boost::function0<bool> f, pre_hooks_)
      if (!f())
        return false;

    BOOST_FOREACH(VariableContainer::ReadRefPair pair, vcon_.read_references)
      solver_->makeAssumption(*pair.second->expr());

    return true;
  }

  // analyse_softconstraints_ analyse the soft constraints and disable conflicting ones
  bool analyse_softconstraints_(bool const exactAnalyse) {

    if (!pre_solve_())
      return false;

    if (exactAnalyse) {

      if (!solver_->analyseSofts())
        return false;

      // TODO get solvable softs
      return true;
    }

    if (solver_->solve())
      return true;

    if (!constraints_.has_soft())
      return false;

    BOOST_FOREACH (ConstraintPtr c, constraints_)
      if (c->is_soft() && c->is_enabled())
        c->disable();

    reset();
    constraints_.set_synced();
    bool result = pre_solve_() && solver_->solve();

    if (!result)
      return false;

    BOOST_FOREACH (ConstraintPtr c, constraints_) {
      if (c->is_soft()) {

        c->enable();
        reset();
        constraints_.set_synced();

        bool enable = solver_->solve();
        result |= enable;
        if (!enable)
          c->disable();
      }
    }

    return result;
  }

  bool solve_() {

    bool result = false;
    if (constraints_.is_changed()) {
      reset();
      constraints_.set_synced();

      result = analyse_softconstraints_(exact_analyse_);
    }

    if (!result && pre_solve_())
      result = solver_->solve();

    if (result) {
      BOOST_FOREACH(VariableContainer::WriteRefPair pair, vcon_.write_references) {
        solver_->read(*vcon_.variables[pair.first], *pair.second);
      }
    }
    return result;
  }

public:
  bool next() {
    return solve_() && vector_gen_.solve_();
  }

  /**
   * read variable "var"
   **/
  template<typename T>
  T operator[](Variable<T> const &var) {

    AssignResultImpl<T> result;
    solver_->read(*vcon_.variables[var.id()], result);
    return result.value();
  }

  std::vector<std::string> get_enabled_softs() const {

    std::vector<std::string> results;

    BOOST_FOREACH (ConstraintPtr c, constraints_)
      if (c->is_enabled() && c->is_soft())
        results.push_back(c->get_name());

    return results;
  }

  std::ostream& print_dot_graph(std::ostream& os, bool const with_softs = false) {

    os << "digraph AST {" << std::endl;
    ToDotVisitor visitor(os);

    BOOST_FOREACH ( ConstraintPtr c , constraints_ ) {
      if (c->is_enabled() && (!c->is_soft() || with_softs))
        c->get_expression()->visit(visitor);
    }

    vector_gen_.print_dot_graph(visitor, with_softs);

    os << "}" << std::endl;
    return os;
  }

private:
  // constraints
  ConstraintSet constraints_;

  // variables
  VariableContainer& vcon_;
  std::vector<boost::function0<bool> > pre_hooks_;

  // solver
  Context ctx_;
  SolverPtr solver_;

  // auxiliary variables
  bool const exact_analyse_;

  // vectors
  VectorGenerator vector_gen_;
};

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

