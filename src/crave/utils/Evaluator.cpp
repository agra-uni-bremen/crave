#include "Evaluator.hpp"

namespace crave {

    void Evaluator::assign(unsigned id, Constant c) {
        assignments_[id] = c;
    }

    bool Evaluator::evaluate(expression const& expr) {
        return visitor_.evaluate(*boost::proto::value(expr));
    }

    uint64_t Evaluator::result() const {
        return result<uint64_t>();
    }
}
