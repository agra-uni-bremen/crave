#include "../crave/backend/VariableGeneratorType.hpp"

namespace crave {

    VariableGenerator::VariableGenerator(VariableContainer* vcon)
    : var_ctn_(vcon), solvers_() {
    }

    void VariableGenerator::reset(std::vector<ConstraintPartition>& partitions) {
        solvers_.clear();

        BOOST_FOREACH(ConstraintPartition& partition, partitions) {
            VarSolverPtr vs(new VariableDefaultSolver(var_ctn_, partition));
            solvers_.push_back(vs);
        }
    }

    bool VariableGenerator::solve() {

        BOOST_FOREACH(VarSolverPtr vs, solvers_) {
            if (!vs->solve()) return false;
        }
        return true;
    }

    std::vector<std::vector<std::string> > VariableGenerator::analyseContradiction() {
        std::vector<std::vector<std::string> > str_vec;

        BOOST_FOREACH(VarSolverPtr vs, solvers_) {
            std::vector<std::vector<std::string> > c = vs->getContradictions();
            if (!c.empty()) str_vec.insert(str_vec.end(), c.begin(), c.end());
        }
        return str_vec;
    }

    std::vector<std::string> VariableGenerator::getInactiveSofts() const {
        std::vector<std::string> str_vec;

        BOOST_FOREACH(VarSolverPtr vs, solvers_) {
            std::vector<std::string> c = vs->getInactiveSofts();
            if (!c.empty()) str_vec.insert(str_vec.end(), c.begin(), c.end());
        }
        return str_vec;
    }

    template <typename T>
    bool VariableGenerator::read(const Variable<T> &var, T* value) const {

        BOOST_FOREACH(VarSolverPtr vs, solvers_) {
            if (vs->read(var, value)) return true;
        }
        return false;
    }
    template bool VariableGenerator::read(const Variable<int> &var, int* value) const;
    template bool VariableGenerator::read(const Variable<unsigned int> &var, unsigned int* value) const;
    template bool VariableGenerator::read(const Variable<char> &var, char* value) const;
    template bool VariableGenerator::read(const Variable<unsigned char> &var, unsigned char* value) const;
    template bool VariableGenerator::read(const Variable<short> &var, short* value) const;
    template bool VariableGenerator::read(const Variable<unsigned short> &var, unsigned short* value) const;
    template bool VariableGenerator::read(const Variable<long> &var, long* value) const;
    template bool VariableGenerator::read(const Variable<unsigned long> &var, unsigned long* value) const;
    template bool VariableGenerator::read(const Variable<long long> &var, long long* value) const;
    template bool VariableGenerator::read(const Variable<unsigned long long> &var, unsigned long long* value) const;
    //SYSTEMC
    //template bool VariableGenerator::read(const Variable<sc_dt::sc_bv<4> > &var, sc_dt::sc_bv<4>* value) const;
    //template bool VariableGenerator::read(const Variable<sc_dt::sc_uint<2> > &var, sc_dt::sc_uint<2>* value) const;
    //template bool VariableGenerator::read(const Variable<sc_dt::sc_int<4> > &var, sc_dt::sc_int<4>* value) const;


}
