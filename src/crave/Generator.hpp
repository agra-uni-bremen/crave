#pragma once

#include "AssignResult.hpp"
#include "Context.hpp"
#include "expression/ToDotNodeVisitor.hpp"
#include "expression/FactoryMetaSMT.hpp"
#include "expression/Node.hpp"
#include "FactoryAssignResult.hpp"

#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

namespace crave {

struct Generator;
typedef Generator DefaultGenerator;

struct Soft_Generator {
  Soft_Generator(Generator & gen) :
      gen(gen) {
  }

  template<typename Expr>
  Soft_Generator & operator()(Expr e);
  Generator & gen;
};

struct Soft;
struct Generator {
  typedef boost::intrusive_ptr<Node> NodePtr;

  Generator()
  : ctx_(), constraints_(), named_constraints_(), disabled_named_constaints_(),
    metaSMT_visitor_(FactoryMetaSMT::newVisitorSWORD()) { }

  template<typename Expr>
  Generator(Expr expr)
  : ctx_(), constraints_(), named_constraints_(), disabled_named_constaints_(),
    metaSMT_visitor_(FactoryMetaSMT::newVisitorSWORD()) {
      (*this)(expr);
    }

  template<typename Expr>
  Generator & operator()(Expr expr) {

    NodePtr n(boost::proto::eval(FixWidth()(expr), ctx_));
    constraints_.push_back(n);
    metaSMT_visitor_->makeAssertion(*n);
    return *this;
  }

  template<typename Expr>
  Generator & operator()(std::string constraint_name, Expr expr) {

    typename std::map<std::string, NodePtr>::iterator ite(
        named_constraints_.lower_bound(constraint_name));

    if (ite != named_constraints_.end() || constraint_name >= ite->first)
      throw std::runtime_error("Constraint already exists.");

    NodePtr nested_expr = boost::proto::eval(FixWidth()(expr), ctx_);

    constraints_.push_back(nested_expr);
    named_constraints_.insert(ite, std::make_pair(constraint_name, nested_expr));
    metaSMT_visitor_->makeAssertion(*nested_expr); // FIXME var_is_enabled -> nested_expr

    return *this;
  }

  std::vector<std::vector<std::string> > analyse_contradiction() {
    //FIXME: return ctx.analyse_contradiction();
  }

  bool enable_constraint(std::string constraint_name) {

    if (0 == named_constraints_.count(constraint_name))
      return false;

    disabled_named_constaints_.erase(constraint_name);
    return true;
  }

  bool disable_constraint(std::string constraint_name) {

    if (0 == named_constraints_.count(constraint_name))
      return false;

    disabled_named_constaints_.insert(constraint_name);
    return true;
  }

  bool is_constraint_enabled(std::string constraint_name) {

    assert (0 != named_constraints_.count(constraint_name));

    return 0 == disabled_named_constaints_.count(constraint_name);
  }

  void add_pre_hook(boost::function0<bool> f) {
    // FIXME: ctx.add_pre_hook(f);
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
    // FIXME: ctx.soft_assertion( FixWidth()(e) );
    return *this;
  }

  /**
   * foreach
   **/
  // FIXME: change IndexVariable to Placeholder
  template<typename value_type, typename Expr>
  Generator & foreach(const __rand_vec <value_type> & v,
      const IndexVariable & i, Expr e) {
    assert(i.id() == _i.id());
    // FIXME
    return *this;
  }

  template<typename value_type, typename Expr>
  Generator & soft_foreach(const __rand_vec <value_type> & v,
      const IndexVariable & i, Expr e) {
    assert(i.id() == _i.id());
    // FIXME
    return *this;
  }

  template<typename value_type, typename Expr>
  Generator & foreach(std::string constraint_name,
      const __rand_vec <value_type> & v, const IndexVariable & i, Expr e) {
    // FIXME
    return *this;
  }

  /**
   * unique & non_unique
   **/
  template<typename value_type>
  Generator & unique(const __rand_vec <value_type> & v) {
    // FIXME
    return *this;
  }

  template<typename value_type>
  Generator & non_unique(const __rand_vec <value_type> & v) {
    // FIXME
    return *this;
  }

  /**
   * generate soft constraints
   **/
  Soft_Generator operator()(Soft const &) {
    return Soft_Generator(*this);
  }

  bool next() {
    // pre_solve()
    for (std::map<std::string, NodePtr>::const_iterator
        it = named_constraints_.begin();
        it != named_constraints_.end(); ++it) {

      if (0 == disabled_named_constaints_.count(it->first))
        metaSMT_visitor_->makeAssumption(*it->second);
    }

    return metaSMT_visitor_->solve();
  }

  /**
   * read variable "var"
   **/
  template<typename T>
  T operator[](Variable<T> const & var) {

    boost::scoped_ptr<AssignResult> result( FactoryAssignResult<T>::newAssignResult() );

    NodePtr node;
    ctx_.getNodeByID(var.id(), node);
    metaSMT_visitor_->read(*node, *result);

    return static_cast<T>(result->value()); // FIXME is there another way to get value?
  }

  void new_disjunction() { /* FIXME: new backend for enums */
  }
  void end_disjunction() { /* FIXME: new backend for enums */
  }
  template<typename Expr>
  void add_to_disjunction(Expr expr) { /* FIXME: new backend for enums */
  }

  std::ostream& print_dot_graph(std::ostream& os) {

    os << "digraph AST {" << std::endl;
    ToDotVisitor visitor(os);

    BOOST_FOREACH ( boost::intrusive_ptr<Node> n, constraints_ )
      n->visit(visitor);
    os << "}" << std::endl;

    return os;
  }

private:
  Context ctx_;
  std::vector<boost::intrusive_ptr<Node> > constraints_;
  std::map<std::string, boost::intrusive_ptr<Node> > named_constraints_;
  std::set<std::string> disabled_named_constaints_;

  boost::scoped_ptr<metaSMTVisitor> metaSMT_visitor_;
//      std::map<int, Context*> vecCtx;
//      std::map<std::string, Context*> ctxOfCstr;
//      std::set<int> uniqueVecSet;
};

template<typename Expr>
Soft_Generator &
Soft_Generator::operator()(Expr e) {
  gen.soft(e);
  return *this;
}

struct Soft {
  Soft_Generator operator()(Generator & gen) const {
    return Soft_Generator(gen);
  }
};

const Soft soft = { };

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

