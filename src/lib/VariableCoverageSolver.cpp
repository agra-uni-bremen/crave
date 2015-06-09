#include "../crave/backend/VariableCoverageSolver.hpp"

namespace crave {

    VariableCoverageSolver::VariableCoverageSolver(VariableContainer *vcon,
            const ConstraintPartition& cp)
    : VariableSolver(vcon, cp) {
        LOG(INFO) << "Create coverage solver for partition " << constr_pttn_;

        BOOST_FOREACH(ConstraintPtr c, constr_pttn_) {
            if (c->isSoft()) {
                continue; // coverage solver ignores soft constraints for now
            }
            if (!c->isCover()) solver_->makeAssertion(*c->expr());
        }
    }

    bool VariableCoverageSolver::solve() {

        BOOST_FOREACH(ConstraintPtr c, constr_pttn_) {
            if (!c->isCover()) continue;
            if (covered_set_.find(c->name()) != covered_set_.end()) {
                continue; // alread covered
            }
            // try solve

            BOOST_FOREACH(VariableContainer::ReadRefPair pair,
                    var_ctn_->read_references) {
                if (constr_pttn_.containsVar(pair.first)) {
                    solver_->makeAssumption(*pair.second->expr());
                }
            }
            solver_->makeAssumption(*c->expr());
            if (solver_->solve()) {
                LOG(INFO) << "Solve partition " << constr_pttn_
                        << " hitting constraint "
                        << c->name();
                covered_set_.insert(c->name());

                BOOST_FOREACH(VariableContainer::WriteRefPair pair,
                        var_ctn_->write_references) {
                    if (constr_pttn_.containsVar(pair.first)) {
                        solver_->read(*var_ctn_->variables[pair.first], *pair.second);
                    }
                }
                return true;
            }
        }
        return false;
    }
}