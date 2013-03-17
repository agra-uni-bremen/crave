#pragma once

#include "AssignResult.hpp"
#include "Context.hpp"
#include "expression/ToDotNodeVisitor.hpp"
#include "expression/FactoryMetaSMT.hpp"
#include "expression/Node.hpp"

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

private:
  typedef std::pair<int, boost::shared_ptr<crave::ReferenceExpression> > ReadRefPair;
  typedef std::pair<int, boost::shared_ptr<crave::AssignResult> > WriteRefPair;

public:
  Generator()
  : constraints_(), named_constraints_(), disabled_named_constaints_(),
    variables_(), read_references_(), write_references_(), pre_hooks_(),
    ctx_(variables_, read_references_, write_references_),
    metaSMT_visitor_(FactoryMetaSMT::newVisitorSWORD()), okay_(false) { }

  template<typename Expr>
  Generator(Expr expr)
  : constraints_(), named_constraints_(), disabled_named_constaints_(),
    variables_(), read_references_(), write_references_(), pre_hooks_(),
    ctx_(variables_, read_references_, write_references_),
    metaSMT_visitor_(FactoryMetaSMT::newVisitorSWORD()), okay_(false) {
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

    if (ite != named_constraints_.end() && constraint_name >= ite->first)
      throw std::runtime_error("Constraint already exists.");

    NodePtr nested_expr = boost::proto::eval(FixWidth()(expr), ctx_);

    constraints_.push_back(nested_expr);
    named_constraints_.insert(ite, std::make_pair(constraint_name, nested_expr));

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

    NodePtr n(boost::proto::eval(FixWidth()(e), ctx_));
    soft_constraints_.push_back(n);
    metaSMT_visitor_->makeSoftAssertion(*n);

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
    BOOST_FOREACH(boost::function0<bool> f, pre_hooks_) {
      metaSMT_visitor_->addPreHook(f);
    }

    for (std::map<std::string, NodePtr>::const_iterator
        it = named_constraints_.begin();
        it != named_constraints_.end(); ++it) {

      if (0 == disabled_named_constaints_.count(it->first))
        metaSMT_visitor_->makeAssumption(*it->second);
    }
    BOOST_FOREACH(ReadRefPair pair, read_references_) {
      metaSMT_visitor_->makeAssumption(*pair.second->expr());
    }

    bool result = metaSMT_visitor_->solve();
    okay_ |= result;
    // post_solve(result)
    if (okay_) {
      BOOST_FOREACH(WriteRefPair pair, write_references_) {
        metaSMT_visitor_->read(*variables_[pair.first], *pair.second);
      }
    }
    return result;
  }

  /**
   * read variable "var"
   **/
  template<typename T>
  T operator[](Variable<T> const &var) {

    AssignResultImpl<T> result;
    metaSMT_visitor_->read(*variables_[var.id()], result);
    return result.value();
  }

  std::ostream& print_dot_graph(std::ostream& os, bool const with_softs = false) {

    os << "digraph AST {" << std::endl;
    ToDotVisitor visitor(os);

    BOOST_FOREACH ( boost::intrusive_ptr<Node> n, constraints_ )
      n->visit(visitor);
    if (with_softs) {
      BOOST_FOREACH ( boost::intrusive_ptr<Node> n, soft_constraints_ )
        n->visit(visitor);
    }
    os << "}" << std::endl;

    return os;
  }

private:
  std::vector<boost::intrusive_ptr<Node> > constraints_;
  std::vector<boost::intrusive_ptr<Node> > soft_constraints_;
  std::map<std::string, boost::intrusive_ptr<Node> > named_constraints_;
  std::set<std::string> disabled_named_constaints_;

  std::map<int, boost::intrusive_ptr<Node> > variables_;
  std::vector<ReadRefPair> read_references_;
  std::vector<WriteRefPair> write_references_;
  std::vector<boost::function0<bool> > pre_hooks_;

  Context ctx_;
  boost::scoped_ptr<metaSMTVisitor> metaSMT_visitor_;

  bool okay_;
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

