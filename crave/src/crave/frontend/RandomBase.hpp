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

#include <map>
#include <string>
#include <vector>
#include "AssignResultToRef.hpp"
#include "Constraint.hpp"
#include "Distribution.hpp"
#include "RandBase.hpp"
#include "RandvInterface.hpp"

namespace crave {

/*
 * Default size of random vector if not otherwise specified by constraints
 */
unsigned int default_rand_vec_size();

/**
 * \ingroup oldAPI
 * \brief Most basic base class of rand_vec<T>.
 */
class __rand_vec_base {
 public:
  /**
   * \brief Destructor.
   */
  virtual ~__rand_vec_base() {}
  /**
   * \brief Sets values of this vector with a given std::vector.
   * 
   * The values are parsed from a std::string in order to get numeral values as well as enum values.
   * 
   * \param values Values to set for this vector.
   */
  virtual void set_values(const std::vector<std::string>&) = 0;

  /**
   * \brief Get the symbolic size of this vector to use in constraints.
   * @return Symbolic size
   */
  virtual Variable<unsigned int> const& size_var() const = 0;

  /**
   * \brief Get the Id of this vector.
   * \return Id of this vector
   */
  virtual int id() const = 0;

  /**
   * \brief Generate random values for a given vector size.
   * 
   * Creates a number of random values for this vector.
   * All values are stored inside the vector.
   * Afterwards the size of this vector ist num.
   * 
   * \param num vector size.
   */
  virtual void gen_values(unsigned num) = 0;
};

extern std::map<int, __rand_vec_base*> vectorBaseMap;

/**
 * \ingroup oldAPI
 * \brief Template specialization of random vectors.
 * 
 * This class distintcts the variable type of the symbol (T1) and the true values type to be stored (T2).
 * In subclasses, T1 and T2 are the same exept for the bool case.
 */
template <typename T1, typename T2>
class __rand_vec_base1 : public __rand_vec_base {
 public:
  /**
   * \brief Empty Constructor.
   */
  __rand_vec_base1() { vectorBaseMap[sym_vec.id()] = this; }

  /**
   * \brief Access to the symbolic vector.
   * \return The whole symbolic vector wrapped by this object.
   */
  const Vector<T1>& operator()() const { return sym_vec; }

  /**
   * \brief Access to an element in the real vector via the array access operator[].
   * \param idx The id to access in the vector
   * \return Value at the given index
   */
  T1& operator[](const int& idx) const { return (T1&)real_vec[idx]; }

  /**
   * \brief Pushes a value into the real vector.
   * 
   * The semantic is the same as std::vector::push_back().
   * 
   * \param x the value to be pushed
   */
  void push_back(const T1& x) { real_vec.push_back(x); }

  /**
   * \brief Removes all values from the internal real vector.
   * 
   * The semantic is the same as std::vector::clear().
   */
  void clear() { real_vec.clear(); }

  /**
   * \brief Return the size of the real vector.
   * \return size of the vector
   */
  std::size_t size() { return real_vec.size(); }

  /**
   * \brief Prints the vector to std::cout.
   */
  void print() {
    std::cout << "vector " << sym_vec.id() << ": ";
    for (unsigned i = 0; i < real_vec.size(); i++) std::cout << real_vec[i] << ", ";
    std::cout << std::endl;
  }

  virtual Variable<unsigned int> const& size_var() const { return sym_vec.size(); }

  virtual void set_values(const std::vector<std::string>& values) {
    T2 tmp;
    AssignResultToRef<T2> result(&tmp);
    real_vec.clear();
    for (unsigned i = 0; i < values.size(); i++) {
      result.set_value(values[i]);
      real_vec.push_back(tmp);
    }
  }

  virtual void gen_values(unsigned num) {
    static randv<T1> r(NULL);
    this->clear();
    for (unsigned i = 0; i < num; i++) {
      r.next();
      this->push_back(r);
    }
  }

  virtual int id() const { return sym_vec.id(); }

 protected:
  Vector<T1> sym_vec;
  std::vector<T2> real_vec;
};

/**
 * \ingroup oldAPI
 * \ingroup newAPI
 * \brief Base class of a random vector.
 * 
 * Simply sets T1 equal T2.
 */
template <typename T>
class __rand_vec : public __rand_vec_base1<T, T> {};

/**
 * \ingroup oldAPI
 * \brief Specialization for boolean random vector.
 */
template <>
class __rand_vec<bool> : public __rand_vec_base1<bool, char> {};

/**
 * \ingroup oldAPI
 * \deprecated As all old API entities, it is recommended to use the \ref newAPI "new API". The equivalent in the new API is \ref crave::crv_vector
 * \brief A vector of randomizable values of Type T.
 * 
 * The random vector is the way to define a vector of randomziable values.
 * Constraints can be defined with random accessed alements or the whole vector.
 */
template <typename T>
class rand_vec : public __rand_vec<T>, public rand_base {
 public:
  /**
   * \brief Constructor with parent object.
   * \param parent Pointer to parent object
   */
  explicit rand_vec(rand_obj_base* parent = 0) : __rand_vec<T>() {
    if (parent != 0) parent->add_base_child(this);
  }

  /**
   * \brief Generate next random values for all elements.
   * 
   * Generates as many random values as defined by default_rand_vec_size().
   * The size of the vector is equal to default_rand_vec_size() afterwards.
   * 
   * \return true
   */
  virtual bool next() {
    __rand_vec<T>::gen_values(default_rand_vec_size());
    return true;
  }

  virtual void gather_values(std::vector<int64_t>* ch) { ch->push_back(this->size()); }

  virtual std::string obj_kind() const { return "rand_vec"; }
};

}  // namespace crave
