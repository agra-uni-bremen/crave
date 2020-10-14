/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/


#pragma once

#include <string>
#include <vector>

#include "Object.hpp"
#include "Variable.hpp"

#include "../backend/Generator.hpp"
#include "../frontend/RandomBase.hpp"

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
