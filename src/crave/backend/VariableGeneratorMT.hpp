#ifndef VARIABLEGENERATORMT_HPP
#define	VARIABLEGENERATORMT_HPP

#include "VariableGeneratorType.hpp"
#include <boost/thread/mutex.hpp>

namespace crave {

    class VariableGeneratorMT : public VariableGenerator {
    public:
        explicit VariableGeneratorMT(VariableContainer* vcon);
        virtual ~VariableGeneratorMT();

        virtual void reset(std::vector<ConstraintPartition>& partitions);
        virtual bool solve();
    private:
        void createNewSolver(ConstraintPartition & partition);
        void solve(VarSolverPtr vs);
        bool solveReturn;
        boost::mutex result_mutex;
        boost::mutex solvers_mutex;

    };
}
#endif	/* VARIABLEGENERATORMT_HPP */

