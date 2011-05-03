#pragma once

#include "Constraint.hpp"
#include "Generator.hpp"

#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <limits>
#include <vector>

namespace platzhalter {

  class rand_base
  {
    protected:
      rand_base() { }

    public:
      virtual bool next() = 0;
  };

  template<typename context_type=Context>
  class rand_obj_of : public rand_base
  {
    public:
      template<typename context>
      rand_obj_of(rand_obj_of<context>* parent = 0) { if (parent != 0) parent->addChild(this); }
      bool next() {
        for (uint i = 0; i < children.size(); i++) 
          if (!children[i]->next()) return false; 
        if (!constraint.next()) return false;
        return true;
      }
      bool enable_constraint(std::string name) { return constraint.enable_constraint(name); }
      bool disable_constraint(std::string name) { return constraint.disable_constraint(name); }
      void addChild(rand_base* rb) { children.push_back(rb); }

    protected:
      rand_obj_of() { }
      std::vector<rand_base*> children;
    
    public:
      Generator<context_type> constraint;
  };

  class rand_obj : public rand_obj_of<> 
  {
    public:
      template<typename context>
      rand_obj(rand_obj_of<context>* parent) 
      : rand_obj_of<>(parent)
      { }
    protected:
      rand_obj() { }
  };

  template<typename T>
  class randv_prim_base : public rand_base
  {
    public:
      operator T() const { return value; }
      friend ostream& operator<<(ostream& os, const randv_prim_base<T>& e) { os << e.value; return os; }
      WriteReference<T>& operator()() { return var; }
      CppType type() { return UNSUPPORTED; }

    protected:
      template<typename context>
      randv_prim_base(rand_obj_of<context>* parent) : var(value) { if (parent != 0) parent->addChild(this); }
      randv_prim_base(const randv_prim_base& other) : var(value), value(other.value) { }
      T value;
      WriteReference<T> var;
  };

  extern boost::mt19937 rng;

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

#define _COMMON_INTERFACE(Typename) \
public: \
  template<typename context> \
  randv(rand_obj_of<context>* parent) : randv_prim_base<Typename>(parent) { } \
  randv(rand_obj* parent) : randv_prim_base<Typename>(parent) { } \
  randv(const randv& other) : randv_prim_base<Typename>(other) { } \
  bool next() { value = (*dist)(rng); return true; } \
  randv<Typename>& operator=(const randv<Typename>& i) { value = i.value; return *this; } \
  randv<Typename>& operator=(Typename i) { value = i; return *this; } \

#define _INTEGER_INTERFACE(Typename) \
public: \
  randv<Typename>& operator++() { ++value;  return *this; } \
  Typename operator++(int) { Typename tmp = value; ++value; return tmp; } \
  randv<Typename>& operator--() { --value;  return *this; } \
  Typename operator--(int) { Typename tmp = value; --value; return tmp; } \
  randv<Typename>& operator+=(Typename i) { value += i;  return *this; } \
  randv<Typename>& operator-=(Typename i) { value -= i;  return *this; } \
  randv<Typename>& operator*=(Typename i) { value *= i;  return *this; } \
  randv<Typename>& operator/=(Typename i) { value /= i;  return *this; } \
  randv<Typename>& operator%=(Typename i) { value %= i;  return *this; } \
  randv<Typename>& operator&=(Typename i) { value &= i;  return *this; } \
  randv<Typename>& operator|=(Typename i) { value |= i;  return *this; } \
  randv<Typename>& operator^=(Typename i) { value ^= i;  return *this; } \
  randv<Typename>& operator<<=(Typename i) { value <<= i;  return *this; } \
  randv<Typename>& operator>>=(Typename i) { value >>= i;  return *this; } \

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

#undef _COMMON_INTERFACE
#undef _INTEGER_INTERFACE
#undef _INTEGER_TYPE

  template<typename T>
  class rand_vec : public __rand_vec<T>, public rand_base
  {  
    public:
      template<typename context>
      rand_vec(rand_obj_of<context>* parent) : __rand_vec<T>() { if (parent != 0) parent->addChild(this); }
      rand_vec(rand_obj* parent) : __rand_vec<T>() { if (parent != 0) parent->addChild(this); }

      bool next() { 
        static randv<unsigned> default_size(NULL);
        default_size.range(5, 10);
        default_size.next();        
        static randv<T> r(NULL);
        for (uint i = 0; i < default_size; i++) {
            r.next();
            push_back(r);
        }
        return true; 
      }
            
  };

} // namespace platzhalter

