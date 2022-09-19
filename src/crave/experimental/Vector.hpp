// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <string>
#include <vector>

#include "../backend/Generator.hpp"
#include "../frontend/RandomBase.hpp"
#include "Object.hpp"
#include "Variable.hpp"

namespace crave {

/**
 * \ingroup newAPI
 * \brief Class for a randomizable vector in the new API.
 *
 * This class represents a vector of randomizable elements in CRAVE.
 */
template <typename T>
class crv_vector : public __rand_vec<T>, public crv_object {
 public:
  /**
   * \brief Define a crv_vector by a name.
   * \param name Name of the vector. "vec" if no name is given.
   */
  crv_vector(crv_object_name = "vec") : __rand_vec<T>() {}

  /**
   * \brief Deleted copy constructor.
   */
  crv_vector(crv_vector const&) = delete;

  std::string obj_kind() const override { return "crv_vector"; }

  /**
   * \brief Generates random values for this vector.
   *
   * The size of the vector is fixed to be default_rand_vec_size()
   * It always returns true.
   *
   * \return true
   */
  bool randomize() override {
    __rand_vec<T>::gen_values(default_rand_vec_size());
    return true;
  }

  void recursive_build(Generator& gen) const override { gen.addVecId(__rand_vec<T>::id()); }
};

}  // namespace crave
