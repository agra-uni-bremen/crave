// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Constraint.hpp"

namespace crave {

/*!
 * \ingroup oldAPI
 * \brief Base class of all CRAVE randomizable components in old API.
 */
class rand_base {
 protected:
  rand_base() {}

  ~rand_base() {}

 public:
  /**
   * \brief Tries to find the next solution.
   *
   * The next solution can be found if there is a combination of values that meet all defined constraints.
   * It is possible to create contradicts with constraints.
   * In that case false is returned.
   * If a solution is found, the variables are set according to the solution and true is returned.
   * \return true if a solution is found, false otherwise
   */
  virtual bool next() = 0;
  /**
   * Retrieves all values from children objects to the given vector.
   * @param vector of all values
   */
  virtual void gather_values(std::vector<int64_t>*) = 0;

  /**
   * \brief Gets the name of this object as a string.
   *
   * The name of the object is most likely the name of the class the object is made of.
   *
   * \return Name of this object as a string
   */
  virtual std::string obj_kind() const = 0;
};

/**
 * \brief Base class for class rand_obj
 */
class rand_obj_base : public rand_base {
 public:
  /**
   * Adds a child object, should not be called by users
   * @param rb child object
   */
  virtual void add_base_child(rand_base* rb) = 0;

  virtual std::string obj_kind() const { return "rand_obj"; }
};

/**
 * \brief Base class for class randv
 */
template <typename T>
class randv_base : public rand_base {
 public:
  /**
   * get value
   * @return value
   */
  operator T() const { return value; }

  /**
   * output stream operator for a randv_base
   * @param os outputstream
   * @param e randv_base object to output
   * @return outputstream
   */
  friend std::ostream& operator<<(std::ostream& os, const randv_base<T>& e) {
    os << e.value;
    return os;
  }

  /**
   * get WriteReference for this object
   * @return WriteReference for this object
   */
  WriteReference<T> const& operator()() const { return var; }

  /**
   * kind of this object as string
   * @return kind of this object
   */
  virtual std::string obj_kind() const { return "randv"; }

  T& value_ref() { return value; }
  const T& value_ref() const { return value; }

 protected:
  explicit randv_base(rand_obj_base* parent) : var(&value) {
    if (parent != 0) parent->add_base_child(this);
  }

  randv_base(const randv_base& other) : var(&value), value(other.value) {}
  WriteReference<T> var;
  T value;
};
}  // namespace crave
