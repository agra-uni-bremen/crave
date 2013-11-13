#pragma once

#include "Constraint.hpp"
#include "Generator.hpp"
#include "Distribution.hpp"

#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <limits>
#include <vector>
#include <set>
#include <cstdarg>

namespace crave {

  void set_global_seed(unsigned int s);
  void set_solver_backend(std::string const&);

  void init(std::string const&);
  void init(std::string const&, std::string const&);

  class rand_base
  {
    protected:
      rand_base() { }
      ~rand_base() { }

    public:
      virtual bool next() = 0;
  };

  class rand_obj : public rand_base
  {
    public:
      rand_obj(rand_obj* pr = 0) { parent = pr; if (parent != 0) { parent->addChild(this, true); } }
      virtual bool next() {
        return constraint.next();
      }
      bool enable_constraint(std::string name) { return constraint.enable_constraint(name); }
      bool disable_constraint(std::string name) { return constraint.disable_constraint(name); }
      bool is_constraint_enabled(std::string name) { return constraint.is_constraint_enabled(name); }
      void addChild(rand_base* rb, bool bindNext) {
        children.push_back(rb);
        if (bindNext) constraint.add_pre_hook(boost::bind<bool>(&rand_base::next, rb));
      }

    protected:
      std::vector<rand_base*> children;
      rand_obj* parent;

    public:
      Generator constraint;
  };

  template<typename T>
  class randv_prim_base : public rand_base
  {
    public:
      operator T() const { return value; }
      friend ostream& operator<<(ostream& os, const randv_prim_base<T>& e) { os << e.value; return os; }
      WriteReference<T> const& operator()() const { return var; }
      CppType type() { return UNSUPPORTED; }

    protected:
      randv_prim_base(rand_obj* parent) : var(value) { if (parent != 0) parent->addChild(this, true); }
      randv_prim_base(const randv_prim_base& other) : var(value), value(other.value) { }
      WriteReference<T> var;
      T value;
  };

  template<typename T>
  class randomize_base
  {
    public:
      void dist(const distribution<T> & dist) { dist_ = dist; }
      void range(T l, T r) { dist_.reset(); dist_(weighted_range<T>(l, r)); }

    protected:
      randomize_base() { }
      T nextValue() { return dist_.nextValue(); }

    protected:
      distribution<T> dist_;
  };

  template<>
  class randomize_base<bool>
  {
    protected:
      randomize_base() { }
      bool nextValue() { return boost::uniform_int<char>(0, 1)(rng); }
  };

  template<typename T>
  class randv : public randv_prim_base<T>, public randomize_base<T>
  {
  public:
    randv(rand_obj* parent) : randv_prim_base<T>(parent) { }
    randv(const randv& other) : randv_prim_base<T>(other) { }
    bool next() { return true; }
  };

#define _COMMON_INTERFACE(Typename) \
public: \
  randv(rand_obj* parent) : randv_prim_base<Typename>(parent) { } \
  randv(const randv& other) : randv_prim_base<Typename>(other) { } \
  bool next() { value = nextValue(); return true; } \
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
  _INTEGER_TYPE(signed char)
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
      rand_vec(rand_obj* parent) : __rand_vec<T>() { if (parent != 0) parent->addChild(this, true); }

      bool next() {
        static randv<unsigned> default_size(NULL);
        default_size.range(5, 10);
        default_size.next();
        static randv<T> r(NULL);
        this->clear();
        for (uint i = 0; i < default_size; i++) {
            r.next();
            this->push_back(r);
        }
        return true;
      }

  };

} // namespace crave

#define INSERT(s, DATA, ELEM) DATA.insert(ELEM);

#define CRAVE_ENUM(name, ...) \
namespace crave { \
  template<> \
  struct randv<name> : randv<int> { \
    randv(rand_obj* parent) : randv<int>(parent) { \
      if (parent == 0) throw std::runtime_error("randv<enum> must be owned by an instance of rand_obj."); \
 \
      std::set<int> s; \
      BOOST_PP_SEQ_FOR_EACH(INSERT, s, __VA_ARGS__); \
      parent->constraint(inside(var, s)); \
    } \
  }; \
}
