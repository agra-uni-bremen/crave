// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <vector>
#include <map>
#include <string>

#include "Constraint.hpp"
#include "Distribution.hpp"

#include "AssignResultImpl.hpp"

namespace crave {

class rand_base {
 protected:
  rand_base() {}
  ~rand_base() {}

 public:
  virtual bool next() = 0;
  virtual void gather_values(std::vector<long long>&) = 0;
};

class rand_obj_base : public rand_base {
 public:
  virtual void add_base_child(rand_base* rb) = 0;
};

template <typename T>
class randv_base : public rand_base {
 public:
  operator T() const { return value; }
  friend ostream& operator<<(ostream& os, const randv_base<T>& e) {
    os << e.value;
    return os;
  }
  WriteReference<T> const& operator()() const { return var; }

 protected:
  explicit randv_base(rand_obj_base* parent) : var(value) {
    if (parent != 0) parent->add_base_child(this);
  }
  randv_base(const randv_base& other) : var(value), value(other.value) {}
  WriteReference<T> var;
  T value;
};

#define RANDV_COMMON_INTERFACE(Typename)                             \
 public:                                                             \
  randv(rand_obj_base* parent = 0) : randv_base<Typename>(parent) {} \
  randv(const randv& other) : randv_base<Typename>(other) {}         \
  randv<Typename>& operator=(const randv<Typename>& i) {             \
    this->value = i.value;                                           \
    return *this;                                                    \
  }                                                                  \
  randv<Typename>& operator=(Typename i) {                           \
    this->value = i;                                                 \
    return *this;                                                    \
  }

#define RANDV_ARITHMETIC_INTERFACE(Typename) \
 public:                                     \
  randv<Typename>& operator++() {            \
    ++(this->value);                         \
    return *this;                            \
  }                                          \
  Typename operator++(int) {                 \
    Typename tmp = this->value;              \
    ++(this->value);                         \
    return tmp;                              \
  }                                          \
  randv<Typename>& operator--() {            \
    --(this->value);                         \
    return *this;                            \
  }                                          \
  Typename operator--(int) {                 \
    Typename tmp = this->value;              \
    --(this->value);                         \
    return tmp;                              \
  }                                          \
  randv<Typename>& operator+=(Typename i) {  \
    this->value += i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator-=(Typename i) {  \
    this->value -= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator*=(Typename i) {  \
    this->value *= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator/=(Typename i) {  \
    this->value /= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator%=(Typename i) {  \
    this->value %= i;                        \
    return *this;                            \
  }

#define RANDV_BITWISE_INTERFACE(Typename)    \
  randv<Typename>& operator&=(Typename i) {  \
    this->value &= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator|=(Typename i) {  \
    this->value |= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator^=(Typename i) {  \
    this->value ^= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator<<=(Typename i) { \
    this->value <<= i;                       \
    return *this;                            \
  }                                          \
  randv<Typename>& operator>>=(Typename i) { \
    this->value >>= i;                       \
    return *this;                            \
  }

#define RANDV_PRIM_INTERFACE(Typename)                                                            \
 public:                                                                                          \
  void gather_values(std::vector<long long>& ch) { ch.push_back(static_cast<long long>(value)); } \
  bool next() {                                                                                   \
    static distribution<Typename> dist;                                                           \
    value = dist.nextValue();                                                                     \
    return true;                                                                                  \
  }

template <>
class randv<bool> : public randv_base<bool> {
  RANDV_COMMON_INTERFACE(bool);
  RANDV_PRIM_INTERFACE(bool);
  RANDV_BITWISE_INTERFACE(bool);
};

// for all C/C++ built-in integer types
#define RANDV_INTEGER_TYPE(typename)                    \
  template <>                                           \
  class randv<typename> : public randv_base<typename> { \
    RANDV_COMMON_INTERFACE(typename);                   \
    RANDV_PRIM_INTERFACE(typename);                     \
    RANDV_ARITHMETIC_INTERFACE(typename);               \
    RANDV_BITWISE_INTERFACE(typename);                  \
  };

RANDV_INTEGER_TYPE(int);
RANDV_INTEGER_TYPE(unsigned int);
RANDV_INTEGER_TYPE(char);
RANDV_INTEGER_TYPE(signed char);
RANDV_INTEGER_TYPE(unsigned char);
RANDV_INTEGER_TYPE(short);
RANDV_INTEGER_TYPE(unsigned short);
RANDV_INTEGER_TYPE(long);
RANDV_INTEGER_TYPE(unsigned long);
RANDV_INTEGER_TYPE(long long);
RANDV_INTEGER_TYPE(unsigned long long);

unsigned int default_rand_vec_size();

class __rand_vec_base {
 public:
  virtual ~__rand_vec_base() {}
  virtual void set_values(std::vector<std::string>&) = 0;
  virtual Variable<unsigned int> const& size_var() const = 0;
};

static std::map<int, __rand_vec_base*> vectorBaseMap;

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
  virtual void set_values(std::vector<std::string>& values) {
    AssignResultImpl<T2> result;
    real_vec.clear();
    for (uint i = 0; i < values.size(); i++) {
      result.set_value(values[i]);
      real_vec.push_back(result.value());
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

  virtual void gather_values(std::vector<long long>& ch) { ch.push_back(this->size()); }
};

}  // namespace crave
