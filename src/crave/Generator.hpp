#pragma once

#include "AssignResult.hpp"
#include "Context.hpp"
#include "Statement.hpp"
#include "VectorConstraint.hpp"
#include "expression/ReplaceVisitor.hpp"
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
  : constraints_(), named_constraints_(), disabled_named_constaints_(), foreach_statements_(),
    soft_foreach_statements_(), named_foreach_statements_(), variables_(),
    vector_variables_(), vectors_(), read_references_(), write_references_(), pre_hooks_(),
    ctx_(variables_, vector_variables_, read_references_, write_references_),
    solver_(NULL), okay_(false), solver_type_() {
      set_backend("SWORD");
  }

  template<typename Expr>
  Generator(Expr expr)
  : constraints_(), named_constraints_(), disabled_named_constaints_(), foreach_statements_(),
    soft_foreach_statements_(), named_foreach_statements_(), variables_(),
    vector_variables_(), vectors_(), read_references_(), write_references_(), pre_hooks_(),
    ctx_(variables_, vector_variables_, read_references_, write_references_),
    solver_(NULL), okay_(false), solver_type_() {
      set_backend("SWORD");
      (*this)(expr);
    }

  Generator(std::string const &type)
  : constraints_(), named_constraints_(), disabled_named_constaints_(), foreach_statements_(),
    soft_foreach_statements_(), named_foreach_statements_(), variables_(),
    vector_variables_(), vectors_(), read_references_(), write_references_(), pre_hooks_(),
    ctx_(variables_, vector_variables_, read_references_, write_references_),
    solver_(NULL), okay_(false), solver_type_() {
      set_backend(type);
  }

  template<typename Expr>
  Generator(std::string const &type, Expr expr)
  : constraints_(), named_constraints_(), disabled_named_constaints_(), foreach_statements_(),
    soft_foreach_statements_(), named_foreach_statements_(), variables_(),
    vector_variables_(), vectors_(), read_references_(), write_references_(), pre_hooks_(),
    ctx_(variables_, vector_variables_, read_references_, write_references_),
    solver_(NULL), okay_(false), solver_type_() {
      set_backend(type);
      (*this)(expr);
  }

  template<typename Expr>
  Generator & operator()(Expr expr) {

    NodePtr n(boost::proto::eval(FixWidth()(expr), ctx_));
    constraints_.push_back(n);
    solver_->makeAssertion(*n);
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

  void set_backend(std::string const& type) {
    solver_type_ = type;
    solver_.reset(FactoryMetaSMT::getInstanceOf(type));
  }

  std::vector<std::vector<std::string> > analyse_contradiction() {
    //FIXME: return ctx.analyse_contradiction();
  }

  bool enable_constraint(std::string constraint_name) {

    if (1 == named_constraints_.count(constraint_name)) {
      disabled_named_constaints_.erase(constraint_name);
      return true;
    }

    typedef std::pair<int, NamedVectorStatement> NamedPair;
    BOOST_FOREACH (NamedPair pair, named_foreach_statements_) {
      if (0 == pair.second.get_name().compare(constraint_name)) {
        disabled_named_constaints_.erase(pair.second.get_name());
        return true;
      }
    }
    return false;
  }

  bool disable_constraint(std::string constraint_name) {

    if (1 == named_constraints_.count(constraint_name)) {
      disabled_named_constaints_.insert(constraint_name);
      return true;
    }

    typedef std::pair<int, NamedVectorStatement> NamedPair;
    BOOST_FOREACH (NamedPair pair, named_foreach_statements_) {
      if (0 == pair.second.get_name().compare(constraint_name)) {
        disabled_named_constaints_.insert(pair.second.get_name());
        return true;
      }
    }
    return false;
  }

  bool is_constraint_enabled(std::string constraint_name) {
    // named_foreach_statements_ or named_constraints_ includes the constraint_name
//     assert (0 != named_constraints_.count(constraint_name));

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
    solver_->makeSoftAssertion(*n);

    return *this;
  }

  /**
   * foreach
   **/
  template<typename value_type, typename Expr>
  Generator & foreach(__rand_vec <value_type> & v,
                      const placeholder & p, Expr e) {

    NodePtr f_expr(boost::proto::eval(FixWidth()(e), ctx_));

    boost::intrusive_ptr<VectorExpr> vec_expr(vector_variables_[v().id()].get());
    Placeholder ph(p.id());

    foreach_statements_.insert(std::make_pair(v().id(), VectorStatement(vec_expr, ph, f_expr)));

    if (0 == vectors_.count(v().id())) {
      vectors_[v().id()] = &v;
      vector_solvers_[v().id()] =
        boost::shared_ptr<metaSMTVisitor>(FactoryMetaSMT::getInstanceOf(solver_type_));
    }
    return *this;
  }

  template<typename value_type, typename Expr>
  Generator & soft_foreach(__rand_vec <value_type> & v,
                           const placeholder & p, Expr e) {

    NodePtr sf_expr(boost::proto::eval(FixWidth()(e), ctx_));

    boost::intrusive_ptr<VectorExpr> vec_expr(vector_variables_[v().id()].get());
    Placeholder ph(p.id());

    soft_foreach_statements_.insert(std::make_pair(v().id(),
                                                   VectorStatement(vec_expr, ph, sf_expr)));

    if (0 == vectors_.count(v().id())) {
      vectors_[v().id()] = &v;
      vector_solvers_[v().id()] =
        boost::shared_ptr<metaSMTVisitor>(FactoryMetaSMT::getInstanceOf(solver_type_));
    }
    return *this;
  }

  template<typename value_type, typename Expr>
  Generator & foreach(std::string constraint_name,
      __rand_vec <value_type> & v, const placeholder & p, Expr e) {

    typedef std::multimap<int, NamedVectorStatement>::iterator nfe_iterator;
    std::pair<nfe_iterator, nfe_iterator> nrange =
        named_foreach_statements_.equal_range(v().id());

    for (nfe_iterator ite = nrange.first; ite != nrange.second; ++ite)
      if (0 == ite->second.get_name().compare(constraint_name))
        throw std::runtime_error("Constraint already exists.");

    NodePtr f_expr(boost::proto::eval(FixWidth()(e), ctx_));
    Placeholder ph(p.id());
    boost::intrusive_ptr<VectorExpr> vec_expr(vector_variables_[v().id()].get());
    NamedVectorStatement vec_stmt(constraint_name, vec_expr, ph, f_expr);

    named_foreach_statements_.insert(nrange.first, std::make_pair(v().id(), vec_stmt));
    if (0 == vectors_.count(v().id())){
      vectors_[v().id()] = &v;
      vector_solvers_[v().id()] =
        boost::shared_ptr<metaSMTVisitor>(FactoryMetaSMT::getInstanceOf(solver_type_));
    }
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

private:
  template<typename Integral>
  bool gen_vec_ (__rand_vec<Integral>* rvp, metaSMTVisitor* solver) {
    __rand_vec<Integral>& vec = *rvp;

    // get size of vector
    unsigned int size;
    if (variables_.find(vec().size().id()) != variables_.end()) {
      AssignResultImpl<unsigned int> ar_size;
      solver_->read(*variables_[vec().size().id()], ar_size);
      size = ar_size.value();
      vec.resize(size);
    } else {
      size = vec.size();
    }

    // get foreach statements of given vector variable
    typedef std::multimap<int, VectorStatement>::iterator fe_iterator;
    std::pair<fe_iterator, fe_iterator> range = foreach_statements_.equal_range(vec().id());

    // initialize VariableExprs for each vector element
    std::vector<NodePtr> variables(size);
    for (unsigned int i = 0; i < size; ++i)
      variables[i] = new VariableExpr(new_var_id(), 1, true);

    ReplaceVisitor replacer(variables);

    // check if an entry exists
    if (range.first != range.second) {

      // replace vector variables and placeholders and make assumptions
      for (fe_iterator ite = range.first; ite != range.second; ++ite) {

        VectorStatement const& fe_statement = ite->second;
        for (unsigned int i = 0; i < size; ++i) {

          replacer.set_vec_idx(i);
          fe_statement.get_expression()->visit(replacer);

          if (replacer.okay())
            solver->makeAssumption(*replacer.result());

          replacer.reset();
        }
      }
    } // end if range.first != range.second

    // add soft constraints to solver
    range = soft_foreach_statements_.equal_range(vec().id());
    if (range.first != range.second) {
      for (fe_iterator ite = range.first; ite != range.second; ++ite) {

        VectorStatement const& fe_statement = ite->second;
        for (unsigned int i = 0; i < size; ++i) {

          replacer.set_vec_idx(i);
          fe_statement.get_expression()->visit(replacer);

          if (replacer.okay())
            solver->makeSoftAssertion(*replacer.result());

          replacer.reset();
        }
      }
    }  // end if

    // add named constraints to solver
    typedef std::multimap<int, NamedVectorStatement>::iterator nfe_iterator;
    std::pair<nfe_iterator, nfe_iterator> nrange =
        named_foreach_statements_.equal_range(vec().id());

    if (nrange.first != nrange.second) {
      for (nfe_iterator ite = nrange.first; ite != nrange.second; ++ite) {

        NamedVectorStatement const& fe_statement = ite->second;
        if (is_constraint_enabled(fe_statement.get_name())) {
          for (unsigned int i = 0; i < size; ++i) {

            replacer.set_vec_idx(i);
            fe_statement.get_expression()->visit(replacer);

            if (replacer.okay())
              solver->makeAssumption(*replacer.result());

            replacer.reset();
          }
        }
      }
    }

    if (!solver->solve())
      return false;

    // read results
    for (unsigned int i = 0; i < size; ++i) {
      AssignResultImpl<Integral> ar_size;
      solver->read(*variables[i], ar_size);
      vec[i] = ar_size.value();
    }
    return true;
  }

  #define _GEN_VEC(typename) if (!gen_vec_(static_cast<__rand_vec<typename>*>(vec_base), solver.get())) return false
  bool solve_vectors_() {

    typedef std::pair<int, NodePtr> VectorVariablePair;
    BOOST_FOREACH(VectorVariablePair p, vector_variables_) {

      int vec_id = p.first;
      NodePtr vec_expr = p.second;
      boost::shared_ptr<metaSMTVisitor> solver = vector_solvers_[vec_id];
      __rand_vec_base* vec_base = vectors_[vec_id];

      switch (vec_base->element_type()) {
        case BOOL: _GEN_VEC(bool); break;
        case INT: _GEN_VEC(int); break;
        case UINT: _GEN_VEC(unsigned int); break;
        case CHAR: _GEN_VEC(char); break;
        case SCHAR: _GEN_VEC(signed char); break;
        case UCHAR: _GEN_VEC(unsigned char); break;
        case SHORT: _GEN_VEC(short); break;
        case USHORT: _GEN_VEC(unsigned short); break;
        case LONG:  _GEN_VEC(long); break;
        case ULONG:  _GEN_VEC(unsigned long); break;
        case LLONG: _GEN_VEC(long long); break;
        case ULLONG: _GEN_VEC(unsigned long long); break;
        default:
          assert(false && "not supported yet");
          return false; // unknown vectors can not be generated
      }

      if (!solver->solve())
        return false;
    }
    return true;
  }
  #undef _GEN_VEC

public:
  bool next() {
    // pre_solve()
    BOOST_FOREACH(boost::function0<bool> f, pre_hooks_) {
      solver_->addPreHook(f);
    }

    for (std::map<std::string, NodePtr>::const_iterator
         it = named_constraints_.begin();
         it != named_constraints_.end(); ++it) {

      if (0 == disabled_named_constaints_.count(it->first))
        solver_->makeAssumption(*it->second);
    }
    BOOST_FOREACH(ReadRefPair pair, read_references_) {
      solver_->makeAssumption(*pair.second->expr());
    }

    bool result = solver_->solve() &&
                  solve_vectors_();
    okay_ |= result;
    // post_solve(result)
    if (okay_) {
      BOOST_FOREACH(WriteRefPair pair, write_references_) {
        solver_->read(*variables_[pair.first], *pair.second);
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
    solver_->read(*variables_[var.id()], result);
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
    typedef std::pair<int const, VectorStatement> ForeachMapPair;
    BOOST_FOREACH ( ForeachMapPair& fp, foreach_statements_ )
      fp.second.get_expression()->visit(visitor);
    os << "}" << std::endl;

    return os;
  }

private:
  // constraints
  std::vector<boost::intrusive_ptr<Node> > constraints_;
  std::vector<boost::intrusive_ptr<Node> > soft_constraints_;
  std::map<std::string, boost::intrusive_ptr<Node> > named_constraints_;
  std::set<std::string> disabled_named_constaints_;
  std::multimap<int, VectorStatement> foreach_statements_;
  std::multimap<int, VectorStatement> soft_foreach_statements_;
  std::multimap<int, NamedVectorStatement> named_foreach_statements_;

  // variables
  std::map<int, boost::intrusive_ptr<Node> > variables_;
  std::map<int, boost::intrusive_ptr<VectorExpr> > vector_variables_;
  std::map<int, __rand_vec_base*> vectors_;
  std::vector<ReadRefPair> read_references_;
  std::vector<WriteRefPair> write_references_;
  std::vector<boost::function0<bool> > pre_hooks_;

  // solver
  Context ctx_;
  boost::scoped_ptr<metaSMTVisitor> solver_;
  std::map<int, boost::shared_ptr<metaSMTVisitor> > vector_solvers_;

  // auxiliary-attributes
  bool okay_;
  std::string solver_type_;
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

