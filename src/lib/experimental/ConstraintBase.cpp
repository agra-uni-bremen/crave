#include "../../crave/experimental/ConstraintBase.hpp"

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
}