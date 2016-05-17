// Copyright 2014 The CRAVE developers. All rights reserved.

#include "../../crave/experimental/Experimental.hpp"

namespace crave {

prev_ prev;

void crv_object::recursive_build(Generator& gen) const {
  for (crv_object* obj : children_) obj->recursive_build(gen);
}

};  // namespace crave
