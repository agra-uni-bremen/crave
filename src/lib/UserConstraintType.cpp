#include "../crave/ir/UserConstraintType.hpp"

namespace crave {

    UserConstraint::UserConstraint(unsigned const id, UserConstraint::expression const expr,
            std::string const& name, std::set<int> support_vars,
            bool const soft, bool const cover,
            bool const enabled)
    : id_(id),
    expr_(expr),
    name_(name),
    support_vars_(support_vars),
    soft_(soft),
    cover_(cover),
    enabled_(enabled) {
    }

    unsigned UserConstraint::id() const {
        return id_;
    }

    UserConstraint::expression const& UserConstraint::expr() const {
        return expr_;
    }

    std::string UserConstraint::name() const {
        return name_;
    }

    bool UserConstraint::isSoft() const {
        return soft_;
    }

    bool UserConstraint::isCover() const {
        return cover_;
    }

    bool UserConstraint::isEnabled() const {
        return enabled_;
    }

    void UserConstraint::enable() {
        enabled_ = true;
    }

    void UserConstraint::disable() {
        enabled_ = false;
    }

    bool UserConstraint::isVectorConstraint() {
        return false;
    }
}
