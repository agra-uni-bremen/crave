#pragma once

#include "AssignResult.hpp"
#include "Context.hpp"
#include "Statement.hpp"
#include "UserConstraint.hpp"
#include "VectorConstraint.hpp"
#include "expression/ReplaceVisitor.hpp"
#include "expression/ToDotNodeVisitor.hpp"
#include "expression/FactoryMetaSMT.hpp"
#include "expression/Node.hpp"

#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
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
  : constraints_(), vector_constraints_(), variables_(), vector_variables_(), vectors_(),
    vector_elements_(), read_references_(), write_references_(), pre_hooks_(),
    ctx_(variables_, vector_variables_, read_references_, write_references_),
    solver_(FactoryMetaSMT::getNewInstance()), constraint_id_(0) {
  }

  template<typename Expr>
  Generator(Expr expr)
  : constraints_(), vector_constraints_(), variables_(), vector_variables_(), vectors_(),
    vector_elements_(), read_references_(), write_references_(), pre_hooks_(),
    ctx_(variables_, vector_variables_, read_references_, write_references_),
    solver_(FactoryMetaSMT::getNewInstance()), constraint_id_(0) {
      (*this)(expr);
    }

  template<typename Expr>
  Generator & operator()(Expr expr) {

    NodePtr n(boost::proto::eval(FixWidth()(expr), ctx_));
    std::string name("constraint_" + boost::lexical_cast<std::string>(constraint_id_++));
    UserConstraint constraint(n, name);

    constraints_.push_back(constraint);
    solver_->makeAssertion(*n);

    return *this;
  }

  template<typename Expr>
  Generator & operator()(std::string constraint_name, Expr expr) {

    BOOST_FOREACH (UserConstraint c, constraints_)
      if (0 == c.get_name().compare(constraint_name))
        throw std::runtime_error("Constraint already exists.");

    NodePtr n(boost::proto::eval(FixWidth()(expr), ctx_));
    UserConstraint constraint(n, constraint_name);

    constraints_.push_back(constraint);
    solver_->makeAssertion(*n);

    return *this;
  }

private:
  void build_solver_() {
    BOOST_FOREACH (UserConstraint const& c, constraints_) {
      if (c.is_enabled()) {
        if (c.is_soft()) {
          solver_->makeSoftAssertion(*c.get_expression());
        } else {
          solver_->makeAssertion(*c.get_expression());
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

    BOOST_FOREACH(UserConstraint c, constraints_)
    {
      s.insert(std::make_pair(s.size(), c.get_expression()));
      out.push_back(c.get_name());
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
    return constraints_.enable_constraint(name);
  }

  bool disable_constraint(std::string const& name) {
    return constraints_.disable_constraint(name);
  }

  bool is_constraint_enabled(std::string const& name) {
    return constraints_.is_constaint_enabled(name);
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
    std::string name("constraint_" + boost::lexical_cast<std::string>(constraint_id_++));
    UserConstraint constraint(n, name, true);

    constraints_.push_back(constraint);
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
        boost::shared_ptr<metaSMTVisitor>(FactoryMetaSMT::getNewInstance());
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
        boost::shared_ptr<metaSMTVisitor>(FactoryMetaSMT::getNewInstance());
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
        boost::shared_ptr<metaSMTVisitor>(FactoryMetaSMT::getNewInstance());
    }
    return *this;
  }

  /**
   * unique & non_unique
   **/
  template<typename value_type>
  Generator & unique(__rand_vec <value_type> & v) {

    if (0 == vectors_.count(v().id())) {
      vectors_[v().id()] = &v;
      vector_solvers_[v().id()] =
        boost::shared_ptr<metaSMTVisitor>(FactoryMetaSMT::getNewInstance());
    }
    unique_vectors_.insert(v().id());
    return *this;
  }

  template<typename value_type>
  Generator & non_unique(const __rand_vec <value_type> & v) {
    unique_vectors_.erase(v().id());
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

    ElementsVector& variables = vector_elements_[vec().id()];
    if (size != variables.size())  {
      variables.resize(size);
      for (unsigned int i = 0; i < size; ++i) {
        boost::intrusive_ptr<VariableExpr> var_expr = new VariableExpr(new_var_id(), 1, true);
        variables[i] = var_expr;
      }
    }

    // get foreach statements of given vector variable
    typedef std::multimap<int, VectorStatement>::iterator fe_iterator;
    std::pair<fe_iterator, fe_iterator> range = foreach_statements_.equal_range(vec().id());

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

    // TODO: substitute the known values

    // make unique
    if (1 == unique_vectors_.count(vec().id())) {
      int i = 0;
      for (ElementsVector::const_iterator var = variables.begin();
           var != variables.end(); ++var, ++i) {
        for (int j = i + 1; j < variables.size(); ++j) {
          solver->makeAssumption(*(new NotEqualOpr(variables[i], variables[j])));
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
    }
    return true;
  }
  #undef _GEN_VEC

public:
  bool next() {
    // pre_solve()
    if (constraints_.is_changed()) {
      reset();
      constraints_.set_synced();
    }

    BOOST_FOREACH(boost::function0<bool> f, pre_hooks_) {
      solver_->addPreHook(f);
    }
    BOOST_FOREACH(ReadRefPair pair, read_references_) {
      solver_->makeAssumption(*pair.second->expr());
    }

    bool result = solver_->solve() &&
                  solve_vectors_();
    if (result) {
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

    BOOST_FOREACH ( UserConstraint c , constraints_ ) {

      std::cout << c.get_name() << " is " << (c.is_enabled()? "enabled": "disabled") << std::endl;

      if (c.is_enabled() && (!c.is_soft() || with_softs))
        c.get_expression()->visit(visitor);
    }

    typedef std::pair<int const, VectorStatement> ForeachMapPair;
    BOOST_FOREACH ( ForeachMapPair& fp, foreach_statements_ )
      fp.second.get_expression()->visit(visitor);

    os << "}" << std::endl;
    return os;
  }

private:
  // typedefs
  typedef std::vector<boost::intrusive_ptr<VariableExpr> > ElementsVector;

  // constraints
  ConstraintSet<UserConstraint> constraints_;
  std::set<int> unique_vectors_;
  std::multimap<int, VectorStatement> foreach_statements_;
  std::multimap<int, VectorStatement> soft_foreach_statements_;
  std::multimap<int, NamedVectorStatement> named_foreach_statements_;

  // variables
  std::map<int, boost::intrusive_ptr<Node> > variables_;
  std::map<int, boost::intrusive_ptr<VectorExpr> > vector_variables_;
  std::map<int, __rand_vec_base*> vectors_;
  std::map<int, ElementsVector> vector_elements_;
  std::vector<ReadRefPair> read_references_;
  std::vector<WriteRefPair> write_references_;
  std::vector<boost::function0<bool> > pre_hooks_;

  // solver
  Context ctx_;
  boost::scoped_ptr<metaSMTVisitor> solver_;
  std::map<int, boost::shared_ptr<metaSMTVisitor> > vector_solvers_;

  // auxiliary-attributes
  mutable unsigned int constraint_id_;
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

