#include "../crave/ir/UserVectorConstraint.hpp"

namespace crave {

    UserVectorConstraint::UserVectorConstraint(unsigned const id, UserVectorConstraint::expression const expr,
            std::string const& name, std::set<int> support_vars,
            bool const unique, bool const soft,
            bool const cover, bool const enabled)
    : UserConstraint(id, expr, name, support_vars, soft, cover, enabled),
    unique_(unique) {
    }

    bool UserVectorConstraint::isUnique() {
        return unique_;
    }

    int UserVectorConstraint::getVectorId() {
        return *support_vars_.begin();
    }

    bool UserVectorConstraint::isVectorConstraint() {
        return true;
    }
}
