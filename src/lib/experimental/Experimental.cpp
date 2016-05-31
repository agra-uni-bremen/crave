// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#include "../../crave/experimental/Experimental.hpp"

namespace crave {

prev_ prev;

void crv_object::recursive_build(Generator& gen) const {
  for (crv_object* obj : children_) obj->recursive_build(gen);
}

};  // namespace crave
