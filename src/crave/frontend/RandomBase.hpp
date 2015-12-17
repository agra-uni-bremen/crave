// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <vector>
#include <map>
#include <string>
#include <ostream>
#include "Constraint.hpp"
#include "Distribution.hpp"
#include "AssignResultToRef.hpp"
#include "RandBase.hpp"
#include "RandvInterface.hpp"

using std::ostream;

namespace crave {

unsigned int default_rand_vec_size();

class __rand_vec_base {
 public:
  virtual ~__rand_vec_base() {}
  virtual void set_values(const std::vector<std::string>&) = 0;
  virtual Variable<unsigned int> const& size_var() const = 0;
};

extern std::map<int, __rand_vec_base*> vectorBaseMap;

template <typename T1, typename T2>
class __rand_vec_base1 : public __rand_vec_base {
 public:
  __rand_vec_base1() { vectorBaseMap[sym_vec.id()] = this; }

  const Vector<T1>& operator()() const { return sym_vec; }

  T1& operator[](const int& idx) const { return (T1&)real_vec[idx]; }

  void push_back(const T1& x) { real_vec.push_back(x); }

  void clear() { real_vec.clear(); }

  std::size_t size() { return real_vec.size(); }

  void print() {
    std::cout << "vector " << sym_vec.id() << ": ";
    for (uint i = 0; i < real_vec.size(); i++) std::cout << real_vec[i] << ", ";
    std::cout << std::endl;
  }

  virtual Variable<unsigned int> const& size_var() const { return sym_vec.size(); }

  virtual void set_values(const std::vector<std::string>& values) {
    T2 tmp;
    AssignResultToRef<T2> result(&tmp);
    real_vec.clear();
    for (uint i = 0; i < values.size(); i++) {
      result.set_value(values[i]);
      real_vec.push_back(tmp);
    }
  }

 protected:
  Vector<T1> sym_vec;
  std::vector<T2> real_vec;
};

template <typename T>
class __rand_vec : public __rand_vec_base1<T, T> {};

template <>
class __rand_vec<bool> : public __rand_vec_base1<bool, char> {};

template <typename T>
class rand_vec : public __rand_vec<T>, public rand_base {
 public:
  explicit rand_vec(rand_obj_base* parent = 0) : __rand_vec<T>() {
    if (parent != 0) parent->add_base_child(this);
  }

  virtual bool next() {
    static randv<T> r(NULL);
    this->clear();
    for (uint i = 0; i < default_rand_vec_size(); i++) {
      r.next();
      this->push_back(r);
    }
    return true;
  }

  virtual void gather_values(std::vector<int64_t>* ch) { ch->push_back(this->size()); }
  
  virtual const char* kind() const { return "rand_vec"; }
};

}  // namespace crave
