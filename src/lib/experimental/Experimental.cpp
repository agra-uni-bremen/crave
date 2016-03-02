// Copyright 2014 The CRAVE developers. All rights reserved.

#include "../../crave/experimental/Experimental.hpp"

namespace crave {

prev_ prev;

void crv_object::recursive_build(Generator& gen) {
  for (crv_object* obj : children_) {
    if (obj->obj_kind() == "crv_constraint") {
      crv_constraint_base* cstr = (crv_constraint_base*)obj;
      if (!cstr->active()) continue;
      unsigned cnt = 0;
      if (!cstr->soft()) {
        for (auto e : cstr->expr_list()) 
          gen(cstr->fullname() + "#" + std::to_string(cnt++), e);
      } else {
        for (auto e : cstr->expr_list())
          gen.soft(cstr->fullname() + "#" + std::to_string(cnt++), e);
      }            
    } else obj->recursive_build(gen);
  }
}

};  // namespace crave
