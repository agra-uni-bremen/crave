#pragma once

#include "Context.hpp"
#include "Logger.hpp"
#include "UserConstraint.hpp"
#include "VariableContainer.hpp"
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
  // typedefs
  typedef boost::shared_ptr<metaSMTVisitor> SolverPtr;
  typedef std::vector<boost::intrusive_ptr<VariableExpr> > ElementsVector;
  typedef std::map<int, ConstraintSet> VectorConstraintsMap;

public:
  Generator()
  : constraints_(), vector_constraints_(), vars_(crave::variables), vectors_(), pre_hooks_(),
    ctx_(vars_), solver_(FactoryMetaSMT::getNewInstance()), exact_analyse_(false), logger_() {
  }

  template<typename Expr>
  Generator(Expr expr)
  : constraints_(), vector_constraints_(), vars_(crave::variables), vectors_(), pre_hooks_(),
    ctx_(vars_), solver_(FactoryMetaSMT::getNewInstance()), exact_analyse_(false), logger_() {
      (*this)(expr);
    }

  template<typename Expr>
  Generator & operator()(Expr expr) {

    constraints_.push_back(ConstraintBuilder::makeConstraint(expr, ctx_));

    return *this;
  }

  template<typename Expr>
  Generator & operator()(std::string constraint_name, Expr expr) {

    BOOST_FOREACH (UserConstraint c, constraints_)
      if (0 == c.get_name().compare(constraint_name))
        throw std::runtime_error("Constraint already exists.");

    constraints_.push_back(ConstraintBuilder::makeConstraint(constraint_name, expr, ctx_));

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
    if (constraints_.is_constraint_enabled(name))
      return true;

    BOOST_FOREACH ( VectorConstraintsMap::value_type& c_pair , vector_constraints_ )
      if (c_pair.second.is_constraint_enabled(name))
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

    constraints_.push_back(ConstraintBuilder::makeConstraint(e, ctx_, true));

    return *this;
  }

  template<typename Expr>
  Generator & soft(std::string name, Expr e) {

    BOOST_FOREACH (UserConstraint c, constraints_)
      if (0 == c.get_name().compare(name))
        throw std::runtime_error("Constraint already exists.");

    constraints_.push_back(ConstraintBuilder::makeConstraint(name, e, ctx_, true));

    return *this;
  }

  /**
   * foreach
   **/
  template<typename value_type, typename Expr>
  Generator & foreach(__rand_vec <value_type> & v,
                      const placeholder & p, Expr e) {

    UserConstraint constraint(ConstraintBuilder::makeConstraint(e, ctx_));

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

    UserConstraint constraint(ConstraintBuilder::makeConstraint(e, ctx_, true));

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

    BOOST_FOREACH ( VectorConstraintsMap::value_type const& c_pair, vector_constraints_ ) {
      BOOST_FOREACH ( UserConstraint const& c, c_pair.second ) {
        if (0 == c.get_name().compare(constraint_name))
          throw std::runtime_error("Constraint already exists.");
      }
    }

    UserConstraint constraint(ConstraintBuilder::makeConstraint(constraint_name, e, ctx_));

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
  void reset_vector_solver_(SolverPtr& solver, int const vec_id, unsigned int const size) {
    solver.reset(FactoryMetaSMT::getNewInstance());
    build_vector_solver_(solver, vec_id, size);
  }

  void build_vector_solver_(SolverPtr& solver, int const vec_id, unsigned int const size) {

    ConstraintSet::VectorElements& vec_elements
        = vector_constraints_[vec_id].get_vec_vars();

    if (vec_elements.size() != size) {
      unsigned int old_size = vec_elements.size();
      vec_elements.resize(size);
      for (unsigned int i = old_size; i < size; ++i)
        vec_elements[i] = new VariableExpr(new_var_id(), 1u, true);
    }

    BOOST_FOREACH ( UserConstraint const& constraint, vector_constraints_[vec_id] ) {

      if (constraint.is_enabled()) {

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

        if (vector_constraints_[vec_id].is_unique()) {
          unsigned int i = 0;

          for (ElementsVector::const_iterator var = vec_elements.begin();
               var != vec_elements.end(); ++var, ++i)
            for (unsigned int j = i + 1; j < vec_elements.size(); ++j)
              solver->makeAssertion(*(new NotEqualOpr(vec_elements[i], vec_elements[j])));
        }
      }
    }
  }

  // analyse_vector_softconstraints_ analyse the soft constraints and disable conflicting ones
  bool analyse_vector_softconstraints_(SolverPtr& solver, unsigned int const vec_id,
                                       unsigned int size, bool const exactAnalyse) {

    if (exactAnalyse) {

      if (!solver->analyseSofts())
        return false;

      // get solvable softs
      return true;
    }

    if (solver->solve())
      return true;

    ConstraintSet& constraints = vector_constraints_[vec_id];

    if (!constraints.has_soft())
      return false;

    BOOST_FOREACH (UserConstraint& c, constraints)
      if (c.is_soft() && c.is_enabled())
        c.disable();

    reset_vector_solver_(solver, vec_id, size);
    constraints.set_synced();
    bool result = solver->solve();

    if (!result)
      return false;

    BOOST_FOREACH (UserConstraint& c, constraints) {
      if (c.is_soft()) {

        c.enable();
        reset_vector_solver_(solver, vec_id, size);
        constraints.set_synced();

        bool enable = solver->solve();
        result |= enable;
        if (!enable)
          c.disable();
      }
    }

    return result;
  }

  template<typename Integral>
  bool gen_vec_ (__rand_vec<Integral>* rvp, SolverPtr& solver) {
    __rand_vec<Integral>& vec = *rvp;

    // get size of vector
    unsigned int size;
    if (vars_.variables_.find(vec().size().id()) != vars_.variables_.end()) {
      AssignResultImpl<unsigned int> ar_size;
      solver_->read(*vars_.variables_[vec().size().id()], ar_size);
      size = ar_size.value();
      vec.resize(size);
    } else {
      size = vec.size();
    }

    bool result = false;
    // build solver for the current vector variable if the vector is changed
    if (vector_constraints_[vec().id()].is_changed()) {
      reset_vector_solver_(solver, vec().id(), size);
      vector_constraints_[vec().id()].set_synced();

      result =
      analyse_vector_softconstraints_(solver, vec().id(), size, exact_analyse_);
    }

    // TODO: substitute the known values
    if (!result)
      result = solver->solve();

    if (result) {
      unsigned int i = 0;
      BOOST_FOREACH ( ConstraintSet::VariablePtr var,
                      vector_constraints_[vec().id()].get_vec_vars() ) {
        AssignResultImpl<Integral> ar_size;
        solver->read(*var, ar_size);
        vec[i++] = ar_size.value();
      }
    }
    return result;
  }

  #define _GEN_VEC(typename) if (!gen_vec_(static_cast<__rand_vec<typename>*>(vec_base), solver)) return false
  bool solve_vectors_() {

    typedef std::pair<int, NodePtr> VectorVariablePair;
    BOOST_FOREACH(VectorVariablePair p, vars_.vector_variables_) {

      int vec_id = p.first;
      NodePtr vec_expr = p.second;
      SolverPtr& solver = vector_solvers_[vec_id];
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

  bool pre_solve_() {
    BOOST_FOREACH(boost::function0<bool> f, pre_hooks_)
      if (!f())
        return false;

    BOOST_FOREACH(VariableContainer::ReadRefPair pair, vars_.read_references_)
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

    BOOST_FOREACH (UserConstraint& c, constraints_)
      if (c.is_soft() && c.is_enabled())
        c.disable();

    reset();
    constraints_.set_synced();
    bool result = pre_solve_() && solver_->solve();

    if (!result)
      return false;

    BOOST_FOREACH (UserConstraint& c, constraints_) {
      if (c.is_soft()) {

        c.enable();
        reset();
        constraints_.set_synced();

        bool enable = solver_->solve();
        result |= enable;
        if (!enable)
          c.disable();
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
      BOOST_FOREACH(VariableContainer::WriteRefPair pair, vars_.write_references_) {
        solver_->read(*vars_.variables_[pair.first], *pair.second);
      }
    }
    return result;
  }

public:
  bool next() {
    return solve_() && solve_vectors_();
  }

  /**
   * read variable "var"
   **/
  template<typename T>
  T operator[](Variable<T> const &var) {

    AssignResultImpl<T> result;
    solver_->read(*vars_.variables_[var.id()], result);
    return result.value();
  }

  std::vector<std::string> get_enabled_softs() const {

    std::vector<std::string> results;

    BOOST_FOREACH (UserConstraint const& c, constraints_)
      if (c.is_enabled() && c.is_soft())
        results.push_back(c.get_name());

    return results;
  }

  std::ostream& print_dot_graph(std::ostream& os, bool const with_softs = false) {

    os << "digraph AST {" << std::endl;
    ToDotVisitor visitor(os);

    BOOST_FOREACH ( UserConstraint const& c , constraints_ ) {
      if (c.is_enabled() && (!c.is_soft() || with_softs))
        c.get_expression()->visit(visitor);
    }

    BOOST_FOREACH ( VectorConstraintsMap::value_type const& fp, vector_constraints_ )
      BOOST_FOREACH ( UserConstraint const& c, fp.second )
        c.get_expression()->visit(visitor);

    os << "}" << std::endl;
    return os;
  }

private:
  // constraints
  ConstraintSet constraints_;
  VectorConstraintsMap vector_constraints_;

  // variables
  VariableContainer& vars_;
  std::map<int, __rand_vec_base*> vectors_;
  std::vector<boost::function0<bool> > pre_hooks_;

  // solver
  Context ctx_;
  SolverPtr solver_;
  std::map<int, SolverPtr> vector_solvers_;

  // auxiliary variables
  bool const exact_analyse_;
  Logger logger_;
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

