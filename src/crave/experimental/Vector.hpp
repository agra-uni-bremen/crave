// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <vector>
#include <string>

#include "Object.hpp"
#include "Variable.hpp"

#include "../frontend/RandomBase.hpp"

namespace crave {

template <typename T>
class crv_vector : public __rand_vec<T>, public crv_object {
 public:
  crv_vector(crv_object_name) : __rand_vec<T>() { }

  std::string kind() override { return "crv_vector"; }

  bool randomize() override {
    __rand_vec<T>::gen_values(default_rand_vec_size());
    return true;
  }
};

}  // namespace crave
