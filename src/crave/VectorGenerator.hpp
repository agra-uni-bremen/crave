#pragma once

#include "RandomBase.hpp"
#include "VariableGenerator.hpp"
#include "expression/ReplaceVisitor.hpp"
#include "expression/FactoryMetaSMT.hpp"
#include "expression/Node.hpp"

#include <boost/foreach.hpp>
#include <boost/intrusive_ptr.hpp>

#include <map>
#include <vector>

namespace crave {

struct VectorSolver {

  typedef boost::intrusive_ptr<VariableExpr> VariablePtr;
  typedef std::vector<VariablePtr> VectorElements;
  
  VectorSolver(int vector_id, VariableGenerator& var_gen) 
  : constraints_(), vector_id_(vector_id), solver_(FactoryMetaSMT::getNewInstance())
  , vec_elements(), var_gen_(var_gen) {  }

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

  bool solve_() {
    __rand_vec_base* vector = vectorBaseMap[vector_id_];

    unsigned int size = default_rand_vec_size();
    if (!var_gen_.read(vector->size_var(), size))
      LOG(INFO) << "Use default size for vector " << vector_id_;
    reset_solver_(size);
    bool result = solver_->solve(false) || solver_->solve(true);
    if (result)
      solver_->readVector(vec_elements, *vector);

    return result;
  }

private:
  std::vector<VectorConstraintPtr> constraints_;
  int vector_id_;
  SolverPtr solver_;
  VectorElements vec_elements;

  VariableGenerator& var_gen_;
};

struct VectorGenerator {

  typedef std::map<int, VectorSolver> VectorSolverMap;

  VectorGenerator(VariableGenerator& var_gen) 
  : vector_solvers_(), var_gen_(var_gen) {  }

  bool solve() {
    BOOST_FOREACH ( VectorSolverMap::value_type& c_pair , vector_solvers_ ) {
      if (!c_pair.second.solve_()) return false;    
    }
    return true;
  }

  void reset(std::vector<VectorConstraintPtr>& v) { 
    vector_solvers_.clear(); 
    BOOST_FOREACH(VectorConstraintPtr vc, v) 
      addConstraint(vc);
  }

private:
  void addConstraint(VectorConstraintPtr vc) {
    int v_id = vc->get_vector_id();
    VectorSolverMap::iterator ite(vector_solvers_.lower_bound(v_id));
    if (ite != vector_solvers_.end() && !(vector_solvers_.key_comp()(v_id, ite->first))) {
        ite->second.addConstraint(vc);
    } else {
      VectorSolver vs(v_id, var_gen_);
      vs.addConstraint(vc);
      vector_solvers_.insert( std::make_pair(v_id, vs) );
    }
  }

private:
  VectorSolverMap vector_solvers_;
  VariableGenerator& var_gen_;
};

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

