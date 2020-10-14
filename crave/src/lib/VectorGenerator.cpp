/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/

#include "../crave/backend/VectorGenerator.hpp"
#include "../crave/utils/Logging.hpp"

#include <string>

namespace crave {
VectorSolver::VectorSolver(int vector_id)
    : constraints_(), vector_id_(vector_id), solver_(FactoryMetaSMT::getNewInstance()), vec_elements_() {}

void VectorSolver::addConstraint(VectorConstraintPtr vc) { constraints_.push_back(vc); }

bool VectorSolver::solve(const VariableGenerator& var_gen) {
  std::string cstr_name_list;
  for(VectorConstraintPtr constraint : constraints_) {
    cstr_name_list += " " + constraint->name();
  }
  LOG(INFO) << "Solve constraints of vector " << vector_id_ << " [" << cstr_name_list << "]";

  __rand_vec_base* vector = vectorBaseMap[vector_id_];

  unsigned int size = default_rand_vec_size();
  if (!var_gen.read(vector->size_var(), &size)) {
    LOG(INFO) << "Use default size for vector " << vector_id_;
  } else {
    LOG(INFO) << "Size of vector " << vector_id_ << " = " << size;
  }
  resetSolver(size);
  bool result = solver_->solve(false) || solver_->solve(true);
  if (result) {
    solver_->readVector(vec_elements_, vector);
    LOG(INFO) << "Done solving vector " << vector_id_;
  }
  else {
    LOG(INFO) << "Failed solving vector " << vector_id_;
  }
  return result;
}

void VectorSolver::resetSolver(unsigned int const size) {
  solver_.reset(FactoryMetaSMT::getNewInstance());
  buildSolver(size);
}

void VectorSolver::buildSolver(unsigned int const size) {
  if (vec_elements_.size() != size) {
    unsigned int old_size = vec_elements_.size();
    vec_elements_.resize(size);
    for (unsigned int i = old_size; i < size; ++i) {
      vec_elements_[i] = new VariableExpr(new_var_id(), 1u, true);
    }
  }

  for(VectorConstraintPtr constraint : constraints_) {
    if (!constraint->isUnique()) {
      ReplaceVisitor replacer(&vec_elements_);
      for (unsigned int i = 0u; i < size; ++i) {
        replacer.setVecIdx(i);
        constraint->expr()->visit(&replacer);

        if (replacer.okay()) {
          if (constraint->isSoft())
            solver_->makeSoftAssertion(*replacer.result());
          else
            solver_->makeAssertion(*replacer.result());
        }

        replacer.reset();
      }
    } else {
      for (unsigned i = 0; i < vec_elements_.size(); i++)
        for (unsigned j = i + 1; j < vec_elements_.size(); ++j) {
          NotEqualOpr neOp(vec_elements_[i], vec_elements_[j]);
          if (constraint->isSoft())
            solver_->makeSoftAssertion(neOp);
          else
            solver_->makeAssertion(neOp);
        }
    }
  }
}

VectorGenerator::VectorGenerator() : vector_solvers_() {}

bool VectorGenerator::solve(const VariableGenerator& var_gen, const std::set<int>& vec_ids) {
  for(VectorSolverMap::value_type & c_pair : vector_solvers_) {
    if (!c_pair.second.solve(var_gen)) return false;
  }
  for(int id : vec_ids) {
    if (vector_solvers_.find(id) != vector_solvers_.end()) continue;
    // unconstrained vector
    __rand_vec_base* vector = vectorBaseMap[id];
    LOG(INFO) << "Generate unconstrained vector " << id;
    unsigned int size = default_rand_vec_size();
    if (!var_gen.read(vector->size_var(), &size)) {
      LOG(INFO) << "Use default size for vector " << id;
    } else {
      LOG(INFO) << "Size of vector " << id << " = " << size;
    }
    vector->gen_values(size);
  }
  return true;
}

void VectorGenerator::reset(const std::vector<VectorConstraintPtr>& v) {
  vector_solvers_.clear();
  for(VectorConstraintPtr vc : v) { addConstraint(vc); }
}

void VectorGenerator::addConstraint(VectorConstraintPtr vc) {
  int v_id = vc->getVectorId();
  VectorSolverMap::iterator ite(vector_solvers_.lower_bound(v_id));
  if (ite != vector_solvers_.end() && !(vector_solvers_.key_comp()(v_id, ite->first))) {
    ite->second.addConstraint(vc);
  } else {
    VectorSolver vs(v_id);
    vs.addConstraint(vc);
    vector_solvers_.insert(std::make_pair(v_id, vs));
  }
}
}
