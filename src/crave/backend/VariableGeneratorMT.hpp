#ifndef VARIABLEGENERATORMT_HPP
#define	VARIABLEGENERATORMT_HPP

#include "VariableGeneratorType.hpp"

namespace crave {

    class VariableGeneratorMT : public VariableGenerator {
    public:
        explicit VariableGeneratorMT(VariableContainer* vcon);
        virtual ~VariableGeneratorMT();

        virtual void reset(std::vector<ConstraintPartition>& partitions);

        virtual bool solve();
        
    private:
        bool solve(VarSolverPtr vs);

    };
}
#endif	/* VARIABLEGENERATORMT_HPP */

