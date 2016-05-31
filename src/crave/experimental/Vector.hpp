// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <vector>
#include <string>

#include "Object.hpp"
#include "Variable.hpp"

#include "../frontend/RandomBase.hpp"
#include "../backend/Generator.hpp"

namespace crave {

template <typename T>
class crv_vector : public __rand_vec<T>, public crv_object {
 public:
  crv_vector(crv_object_name = "vec") : __rand_vec<T>() { }

  crv_vector(crv_vector const &) = delete;

  std::string obj_kind() const override { return "crv_vector"; }

  bool randomize() override {
    __rand_vec<T>::gen_values(default_rand_vec_size());
    return true;
  }

  void recursive_build(Generator& gen) const override { gen.addVecId(__rand_vec<T>::id()); }
};

}  // namespace crave
