#pragma once

#include <limits>

#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "Constraint.hpp"
#include "Context.hpp"

namespace platzhalter {

  class rand_base
  {
    protected:
      rand_base() { }

    public:
      virtual bool next() = 0;
  };

  class rand_obj : public rand_base
  {
    public:
      rand_obj(rand_obj* parent) { if (parent != 0) parent->addChild(this); }
      bool next() {
        for (uint i = 0; i < children.size(); i++) 
          if (!children[i]->next()) return false; 
        if (!constraint.next()) return false;
        return true;
      }
      void addChild(rand_base* rb) { children.push_back(rb); }

    protected:
      std::vector<rand_base*> children;
      Generator<> constraint;
  };

  template<typename T>
  class randv_prim_base : public rand_base
  {
    protected:
      randv_prim_base(rand_obj* parent) : var(value) { if (parent != 0) parent->addChild(this); }
      T value;
      WriteReference<T> var;
  };

  boost::mt19937 rng;

  template<typename T>
  class randomize_base
  {
    public:
      void range(T _min, T _max) { min = _min; max = _max; dist = new boost::uniform_int<T>(min, max); }

    protected:
      randomize_base() { range(std::numeric_limits<T>::min(), std::numeric_limits<T>::max()); }
      T min;
      T max;
      boost::uniform_int<T>* dist;
  };

  template<>
  class randomize_base<bool>
  {
    protected:
      randomize_base() : dist(new boost::uniform_int<char>(0, 1)) { }
      boost::uniform_int<char> *dist;
  };

#define _COMMON_INTERFACE(typename) \
public: \
  randv(rand_obj* parent) : randv_prim_base<typename>(parent) { } \
  randv<typename>& operator=(const randv<typename>& i) { value = i.value; return *this; } \
  randv<typename>& operator=(typename i) { value = i; return *this; } \
  operator typename() const { return value; } \
  friend ostream& operator<<(ostream& os, const randv<typename>& e) { os << e.value; return os; } \
  WriteReference<typename>& operator()() { return var; } \
  bool next() { value = (*dist)(rng); return true; }

#define _INTEGER_INTERFACE(typename) \
public: \
  randv<typename>& operator++() { ++value;  return *this; } \
  typename operator++(int) { typename tmp = value; ++value; return tmp; } \
  randv<typename>& operator--() { --value;  return *this; } \
  typename operator--(int) { typename tmp = value; --value; return tmp; } \
  randv<typename>& operator+=(typename i) { value += i;  return *this; } \
  randv<typename>& operator-=(typename i) { value -= i;  return *this; } \
  randv<typename>& operator*=(typename i) { value *= i;  return *this; } \
  randv<typename>& operator/=(typename i) { value /= i;  return *this; } \
  randv<typename>& operator%=(typename i) { value %= i;  return *this; } \
  randv<typename>& operator&=(typename i) { value &= i;  return *this; } \
  randv<typename>& operator|=(typename i) { value |= i;  return *this; } \
  randv<typename>& operator^=(typename i) { value ^= i;  return *this; } \
  randv<typename>& operator<<=(typename i) { value <<= i;  return *this; } \
  randv<typename>& operator>>=(typename i) { value >>= i;  return *this; } \

// bool
  template<>
  class randv<bool> : public randv_prim_base<bool>, public randomize_base<bool>
  {
    _COMMON_INTERFACE(bool)
  };

// for all C/C++ built-in integer types
#define _INTEGER_TYPE(typename) \
template<> \
class randv<typename> : public randv_prim_base<typename>, public randomize_base<typename> \
{ \
  _COMMON_INTERFACE(typename) \
  _INTEGER_INTERFACE(typename) \
}; \

  _INTEGER_TYPE(int)
  _INTEGER_TYPE(unsigned int)
  _INTEGER_TYPE(char)
  _INTEGER_TYPE(unsigned char)
  _INTEGER_TYPE(short)
  _INTEGER_TYPE(unsigned short)
  _INTEGER_TYPE(long)
  _INTEGER_TYPE(unsigned long)
  _INTEGER_TYPE(long long)
  _INTEGER_TYPE(unsigned long long)

} // namespace platzhalter

