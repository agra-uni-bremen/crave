#pragma once

#include "AssignResult.hpp"
#include "Context.hpp"
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
  // typedefs
  typedef std::vector<boost::intrusive_ptr<VariableExpr> > ElementsVector;
  typedef boost::shared_ptr<metaSMTVisitor> VectorSolverPtr;
  typedef std::map<int, ConstraintSet<VectorConstraint> > VectorConstraintsMap;

  typedef std::pair<int, boost::shared_ptr<crave::ReferenceExpression> > ReadRefPair;
  typedef std::pair<int, boost::shared_ptr<crave::AssignResult> > WriteRefPair;

public:
  Generator()
  : constraints_(), vector_constraints_(), variables_(), vector_variables_(), vectors_(),
    read_references_(), write_references_(), pre_hooks_(),
    ctx_(variables_, vector_variables_, read_references_, write_references_),
    solver_(FactoryMetaSMT::getNewInstance()), constraint_id_(0) {
  }

  template<typename Expr>
  Generator(Expr expr)
  : constraints_(), vector_constraints_(), variables_(), vector_variables_(), vectors_(),
    read_references_(), write_references_(), pre_hooks_(),
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
    if (constraints_.enable_constraint(name))
      return true;

    BOOST_FOREACH ( VectorConstraintsMap::value_type& c_pair , vector_constraints_ )
      if (c_pair.second.enable_constraint(name))
        return true;
    return false;
  }

  bool disable_constraint(std::string const& name) {
    if (constraints_.disable_constraint(name))
      return true;

    BOOST_FOREACH ( VectorConstraintsMap::value_type& c_pair , vector_constraints_ )
      if (c_pair.second.disable_constraint(name))
        return true;
    return false;
  }

  bool is_constraint_enabled(std::string const& name) {
    if (constraints_.is_constaint_enabled(name))
      return true;

    BOOST_FOREACH ( VectorConstraintsMap::value_type& c_pair , vector_constraints_ )
      if (c_pair.second.is_constaint_enabled(name))
        return true;
    return false;
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

    return *this;
  }

  /**
   * foreach
   **/
  template<typename value_type, typename Expr>
  Generator & foreach(__rand_vec <value_type> & v,
                      const placeholder & p, Expr e) {

    NodePtr vec_expr(boost::proto::eval(FixWidth()(e), ctx_));
    std::string name("constraint_" + boost::lexical_cast<std::string>(constraint_id_++));
    VectorConstraint constraint(vec_expr, name);

    VectorConstraintsMap::iterator ite(vector_constraints_.lower_bound(v().id()));
    if (ite != vector_constraints_.end() && !(vector_constraints_.key_comp()(v().id(), ite->first))) {
      // v already exists
      ite->second.push_back(constraint);

    } else {

      vector_constraints_[v().id()].push_back(constraint);
      vectors_[v().id()] = &v;
      vector_solvers_[v().id()] =
        boost::shared_ptr<metaSMTVisitor>(FactoryMetaSMT::getNewInstance());

    }
    return *this;
  }

  template<typename value_type, typename Expr>
  Generator & soft_foreach(__rand_vec <value_type> & v,
                           const placeholder & p, Expr e) {

    NodePtr vec_expr(boost::proto::eval(FixWidth()(e), ctx_));
    std::string name("constraint_" + boost::lexical_cast<std::string>(constraint_id_++));
    VectorConstraint constraint(vec_expr, name, true);

    VectorConstraintsMap::iterator ite(vector_constraints_.lower_bound(v().id()));
    if (ite != vector_constraints_.end() && !(vector_constraints_.key_comp()(v().id(), ite->first))) {
      // v already exists
      ite->second.push_back(constraint);

    } else {

      vector_constraints_[v().id()].push_back(constraint);
      vectors_[v().id()] = &v;
      vector_solvers_[v().id()] =
        boost::shared_ptr<metaSMTVisitor>(FactoryMetaSMT::getNewInstance());

    }
    return *this;
  }

  template<typename value_type, typename Expr>
  Generator & foreach(std::string constraint_name,
      __rand_vec <value_type> & v, const placeholder & p, Expr e) {

    BOOST_FOREACH ( VectorConstraintsMap::value_type c_pair, vector_constraints_ ) {
      BOOST_FOREACH ( VectorConstraint c, c_pair.second ) {
        if (0 == c.get_name().compare(constraint_name))
          throw std::runtime_error("Constraint already exists.");
      }
    }

    NodePtr vec_expr(boost::proto::eval(FixWidth()(e), ctx_));
    VectorConstraint constraint(vec_expr, constraint_name);

    VectorConstraintsMap::iterator ite(vector_constraints_.lower_bound(v().id()));
    if (ite != vector_constraints_.end() &&
        !(vector_constraints_.key_comp()(v().id(), ite->first))) {
      // v already exists
      ite->second.push_back(constraint);

    } else {

      vector_constraints_[v().id()].push_back(constraint);
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
    vector_constraints_[v().id()].set_unique(true);
    return *this;
  }

  template<typename value_type>
  Generator & non_unique(const __rand_vec <value_type> & v) {
    vector_constraints_[v().id()].set_unique(false);
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
  bool gen_vec_ (__rand_vec<Integral>* rvp, VectorSolverPtr solver) {
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

    // build solver for the current vector variable if the vector is changed
    if (vector_constraints_[vec().id()].is_changed()) {
      solver.reset(FactoryMetaSMT::getNewInstance());

      ConstraintSet<VectorConstraint>::VectorElements& vec_elements
          = vector_constraints_[vec().id()].get_vec_vars();

      if (vec_elements.size() != size) {
        unsigned int old_size = vec_elements.size();
        vec_elements.resize(size);
        for (unsigned int i = old_size; i < size; ++i)
          vec_elements[i] = new VariableExpr(new_var_id(), 1u, true);
      }

      BOOST_FOREACH ( VectorConstraint constraint, vector_constraints_[vec().id()] ) {

        if (constraint.is_enabled()) {

          VectorConstraint::ExpressionsVector& vec_expressions
              = constraint.get_exprs();

          if (vec_expressions.size() != size) {
            unsigned int old_size = vec_expressions.size();
            vec_expressions.resize(size);
            for (unsigned int i = old_size; i < size; ++i)
              vec_expressions[i] = new Constant(true);
          }

          ReplaceVisitor replacer(vec_elements);
          for (unsigned int i = 0u; i < size; ++i) {

            replacer.set_vec_idx(i);
            constraint.get_expression()->visit(replacer);

            if (replacer.okay()) {
              if (constraint.is_soft())
                solver->makeSoftAssertion(*replacer.result());
              else
                solver->makeAssertion(*replacer.result());
            }

            replacer.reset();
          }

          if (vector_constraints_[vec().id()].is_unique()) {
            unsigned int i = 0;

            for (ElementsVector::const_iterator var = vec_elements.begin();
                 var != vec_elements.end(); ++var, ++i)
              for (unsigned int j = i + 1; j < vec_elements.size(); ++j)
                solver->makeAssertion(*(new NotEqualOpr(vec_elements[i], vec_elements[j])));
          }
        }
      }
    }
    // TODO: substitute the known values
    if (!solver->solve())
      return false;

    unsigned int i = 0;
    BOOST_FOREACH ( VectorConstraint::VariablePtr var,
                    vector_constraints_[vec().id()].get_vec_vars() ) {
      AssignResultImpl<Integral> ar_size;
      solver->read(*var, ar_size);
      vec[i++] = ar_size.value();
    }
    return true;
  }

  #define _GEN_VEC(typename) if (!gen_vec_(static_cast<__rand_vec<typename>*>(vec_base), solver)) return false
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
      if (c.is_enabled() && (!c.is_soft() || with_softs))
        c.get_expression()->visit(visitor);
    }

    typedef std::pair<int const, ConstraintSet<VectorConstraint> > ForeachMapPair;
    BOOST_FOREACH ( ForeachMapPair& fp, vector_constraints_ )
      BOOST_FOREACH ( VectorConstraint c, fp.second )
        c.get_expression()->visit(visitor);

    os << "}" << std::endl;
    return os;
  }

private:
  // constraints
  ConstraintSet<UserConstraint> constraints_;
  VectorConstraintsMap vector_constraints_;

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
  std::map<int, VectorSolverPtr> vector_solvers_;

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

