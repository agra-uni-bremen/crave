#include "../../crave/experimental/ConstraintBase.hpp"
#include "../../crave/backend/Generator.hpp"

namespace crave {
  crv_constraint_base::crv_constraint_base() : active_(true) {}
    
  std::string crv_constraint_base::obj_kind() const { return "crv_constraint"; }

  void crv_constraint_base::activate() {
    if (!active_) {
      active_ = true;
      request_rebuild();
    }
  }

  void crv_constraint_base::deactivate() {
    if (active_) {
      active_ = false;
      request_rebuild();
    }
  }

  bool crv_constraint_base::active() const { return active_; }

  void crv_constraint_base::recursive_build(Generator &gen) const {
    if (!active()) return;
    unsigned cnt = 0;
    if (!soft()) {
      for (auto e : expr_list()) gen(fullname() + "#" + std::to_string(cnt++), e);
    } else {
      for (auto e : expr_list()) gen.soft(fullname() + "#" + std::to_string(cnt++), e);
    }
  }
}
