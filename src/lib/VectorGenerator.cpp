#include "../crave/backend/VectorGenerator.hpp"

namespace crave {
VectorSolver::VectorSolver(int vector_id)
    : constraints_(), vector_id_(vector_id), solver_(FactoryMetaSMT::getNewInstance()), vec_elements_() {}

void VectorSolver::addConstraint(VectorConstraintPtr vc) { constraints_.push_back(vc); }

bool VectorSolver::solve(const VariableGenerator& var_gen) {
  __rand_vec_base* vector = vectorBaseMap[vector_id_];

  unsigned int size = default_rand_vec_size();
  if (!var_gen.read(vector->size_var(), &size)) {
    LOG(INFO) << "Use default size for vector " << vector_id_;
  }
  resetSolver(size);
  bool result = solver_->solve(false) || solver_->solve(true);
  if (result) solver_->readVector(vec_elements_, vector);

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

  BOOST_FOREACH(VectorConstraintPtr constraint, constraints_) {
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
      for (uint i = 0; i < vec_elements_.size(); i++)
        for (uint j = i + 1; j < vec_elements_.size(); ++j) {
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

bool VectorGenerator::solve(const VariableGenerator& var_gen) {
  BOOST_FOREACH(VectorSolverMap::value_type & c_pair, vector_solvers_) {
    if (!c_pair.second.solve(var_gen)) return false;
  }
  return true;
}

void VectorGenerator::reset(const std::vector<VectorConstraintPtr>& v) {
  vector_solvers_.clear();
  BOOST_FOREACH(VectorConstraintPtr vc, v) { addConstraint(vc); }
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
