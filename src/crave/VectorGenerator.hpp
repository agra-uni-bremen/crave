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

struct VectorSolver {

  typedef boost::intrusive_ptr<VariableExpr> VariablePtr;
  typedef std::vector<VariablePtr> VectorElements;
  
  VectorSolver(int vector_id, VariableContainer& vars, SolverPtr& main_solver, bool exact_analyse) 
  : constraints_(), vector_id_(vector_id), solver_(FactoryMetaSMT::getNewInstance()), vec_elements()
  , vcon_(vars), main_solver_(main_solver), exact_analyse_(exact_analyse) {  }

  void addConstraint(VectorConstraintPtr vc) {
    constraints_.push_back(vc);
  }

  void reset_solver_(unsigned int const size) {
    solver_.reset(FactoryMetaSMT::getNewInstance());
    build_solver_(size);
  }

  void build_solver_(unsigned int const size) {

    if (vec_elements.size() != size) {
      unsigned int old_size = vec_elements.size();
      vec_elements.resize(size);
      for (unsigned int i = old_size; i < size; ++i)
        vec_elements[i] = new VariableExpr(new_var_id(), 1u, true);
    }

    BOOST_FOREACH ( VectorConstraintPtr constraint, constraints_ ) {

      if (!constraint->is_unique()) {

        ReplaceVisitor replacer(vec_elements);
        for (unsigned int i = 0u; i < size; ++i) {

          replacer.set_vec_idx(i);
          constraint->get_expression()->visit(replacer);
  
          if (replacer.okay()) {
            if (constraint->is_soft())
              solver_->makeSoftAssertion(*replacer.result());
            else
              solver_->makeAssertion(*replacer.result());
          }

          replacer.reset();
        }
      }
      else {
        for (uint i = 0; i < vec_elements.size(); i++)
          for (uint j = i + 1; j < vec_elements.size(); ++j)
            if (constraint->is_soft())
              solver_->makeSoftAssertion(*(new NotEqualOpr(vec_elements[i], vec_elements[j])));
            else   
              solver_->makeAssertion(*(new NotEqualOpr(vec_elements[i], vec_elements[j])));
      }
    }
  }

  template<typename Integral>
  bool gen_vec_ (__rand_vec<Integral>* rvp) {
    __rand_vec<Integral>& vec = *rvp;

    // get size of vector
    unsigned int size;
    if (vcon_.variables.find(vec().size().id()) != vcon_.variables.end()) {
      AssignResultImpl<unsigned int> ar_size;
      main_solver_->read(*vcon_.variables[vec().size().id()], ar_size);
      size = ar_size.value();
      vec.resize(size);
    } else {
      size = vec.size();
    }

    reset_solver_(size);
    bool result = solver_->solve() ? true : solver_->solve(true);
    if (result) {
      unsigned int i = 0;
      BOOST_FOREACH ( VariablePtr var,
                      vec_elements ) {
        AssignResultImpl<Integral> val;
        solver_->read(*var, val);
        vec[i++] = val.value();
      }
    }
    return result;
  }

  #define _GEN_VEC(typename) if (!gen_vec_(static_cast<__rand_vec<typename>*>(vector_))) return false
  bool solve_() {
    __rand_vec_base* vector_ = vectorBaseMap[vector_id_];
    switch (vector_->element_type()) {
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
    return true;
  }
  #undef _GEN_VEC

private:
  std::vector<VectorConstraintPtr> constraints_;
  int vector_id_;
  SolverPtr solver_;
  VectorElements vec_elements;

  VariableContainer& vcon_;
  SolverPtr& main_solver_;
  bool const exact_analyse_;
};

struct VectorGenerator {

  typedef std::map<int, VectorSolver> VectorSolverMap;
  
  VectorGenerator( VariableContainer& vars, SolverPtr& main_solver, bool exact_analyse) 
  : vector_solvers_(), vcon_(vars), main_solver_(main_solver), exact_analyse_(exact_analyse) {  }

  void addConstraint(VectorConstraintPtr vc) {
    int v_id = vc->get_vector_id();
    VectorSolverMap::iterator ite(vector_solvers_.lower_bound(v_id));
    if (ite != vector_solvers_.end() && !(vector_solvers_.key_comp()(v_id, ite->first))) {
        ite->second.addConstraint(vc);
    } else {
      VectorSolver vs(v_id, vcon_, main_solver_, exact_analyse_);
      vs.addConstraint(vc);
      vector_solvers_.insert( std::make_pair(v_id, vs) );
    }
  }

  bool solve_() {
    BOOST_FOREACH ( VectorSolverMap::value_type& c_pair , vector_solvers_ ) {
      if (!c_pair.second.solve_()) return false;    
    }
    return true;
  }

  void reset() { vector_solvers_.clear(); }

private:
  VectorSolverMap vector_solvers_;
  VariableContainer& vcon_;
  SolverPtr& main_solver_;
  bool const exact_analyse_;
};

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

