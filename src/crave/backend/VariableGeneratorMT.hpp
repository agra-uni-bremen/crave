#ifndef VARIABLEGENERATORMT_HPP
#define	VARIABLEGENERATORMT_HPP

#include "VariableGeneratorType.hpp"
#include <boost/thread/mutex.hpp>

namespace crave {

    class VariableGeneratorMT : public VariableGenerator {
    public:
        explicit VariableGeneratorMT(VariableContainer const& vcon);
        virtual ~VariableGeneratorMT();

        virtual void reset(std::vector<ConstraintPartition>& partitions);
        virtual bool solve();
    private:
        void createNewSolver(ConstraintPartition & partition,unsigned int index);
        void solve(VarSolverPtr vs);
        bool solveReturn;
        boost::shared_ptr<VariableSolver>* var_solv_array;
        boost::mutex result_mutex;

    };
}
#endif	/* VARIABLEGENERATORMT_HPP */

