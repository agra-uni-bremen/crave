#pragma once

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

struct VectorGenerator {

  typedef std::map<int, ConstraintSet> VectorConstraintsMap;
  
  VectorGenerator( VariableContainer& vars, SolverPtr& solver, bool exact_analyse) 
  : vector_constraints_(), vectors_(), vars_(vars), solver_(solver), exact_analyse_(exact_analyse) {  }

  bool enable_constraint(std::string const& name) {
    BOOST_FOREACH ( VectorConstraintsMap::value_type& c_pair , vector_constraints_ )
      if (c_pair.second.enable_constraint(name))
        return true;
    return false;
  }

  bool disable_constraint(std::string const& name) {
    BOOST_FOREACH ( VectorConstraintsMap::value_type& c_pair , vector_constraints_ )
      if (c_pair.second.disable_constraint(name))
        return true;
    return false;
  }

  bool is_constraint_enabled(std::string const& name) {
    BOOST_FOREACH ( VectorConstraintsMap::value_type& c_pair , vector_constraints_ )
      if (c_pair.second.is_constraint_enabled(name))
        return true;
    return false;
  }

  template<typename value_type>
  void addForEach(UserConstraint & constraint, __rand_vec <value_type> & v) {
    BOOST_FOREACH ( VectorConstraintsMap::value_type const& c_pair, vector_constraints_ ) {
      BOOST_FOREACH ( UserConstraint const& c, c_pair.second ) {
        if (0 == c.get_name().compare(constraint.get_name()))
          throw std::runtime_error("Constraint already exists.");
      }
    }
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
  }

  template<typename value_type>
  void setUnique(const __rand_vec <value_type> & v, bool b) {
    vector_constraints_[v().id()].set_unique(b);
  }

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
          for (uint i = 0; i < vec_elements.size(); i++)
            for (uint j = i + 1; j < vec_elements.size(); ++j)
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
  bool solve_() {

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

  void print_dot_graph(ToDotVisitor& visitor, bool const with_softs = false) {
    BOOST_FOREACH ( VectorConstraintsMap::value_type const& fp, vector_constraints_ )
      BOOST_FOREACH ( UserConstraint const& c, fp.second )
        c.get_expression()->visit(visitor);
  }

private:
  VectorConstraintsMap vector_constraints_;
  std::map<int, __rand_vec_base*> vectors_;
  std::map<int, SolverPtr> vector_solvers_;

  VariableContainer& vars_;
  SolverPtr& solver_;
  bool const exact_analyse_;
};

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

