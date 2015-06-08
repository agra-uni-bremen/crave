#include "VariableCoverageGenerator.hpp"

namespace crave {

    VariableCoverageGenerator::VariableCoverageGenerator(VariableContainer* vcon)
    : VariableGenerator(vcon) {
    }

    void VariableCoverageGenerator::reset(std::vector<ConstraintPartition>& partitions) {
        solvers_.clear();

        BOOST_FOREACH(ConstraintPartition& partition, partitions) {
            VarSolverPtr vs(new VariableCoverageSolver(var_ctn_, partition));
            solvers_.push_back(vs);
        }
    }

    virtual bool VariableCoverageGenerator::solve() {
        bool res = false;

        BOOST_FOREACH(VarSolverPtr vs, solvers_) {
            if (vs->solve())
                res = true; // one solver has hitted an uncovered constraint
            // -> everything ok,
            // from covered solvers, the last
            // solutions will be reused.
        }
        return res;
    }
}
