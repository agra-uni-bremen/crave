#pragma once

#include "Constraint.hpp"
#include "Generator.hpp"

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
      rand_obj(rand_obj* parent = 0) { if (parent != 0) parent->addChild(this); }
      virtual bool next() {
        return constraint.next();
      }
      bool enable_constraint(std::string name) { return constraint.enable_constraint(name); }
      bool disable_constraint(std::string name) { return constraint.disable_constraint(name); }
      bool is_constraint_enabled(std::string name) { return constraint.is_constraint_enabled(name); }
      void addChild(rand_base* rb) {
        children.push_back(rb);
        constraint.add_pre_hook(boost::bind<bool>(&rand_base::next, rb));
      }

    protected:
      std::vector<rand_base*> children;

    public:
      Generator constraint;
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
      randv_prim_base(rand_obj* parent) : var(value) { if (parent != 0) parent->addChild(this); }
      randv_prim_base(const randv_prim_base& other) : var(value), value(other.value) { }
      WriteReference<T> var;
      T value;
  };

  extern boost::mt19937 rng;

  void set_global_seed(unsigned int s);
  void set_solver_backend(std::string const&);

  template<typename T>
  struct weighted_range
  {
    weighted_range(T l, T r, unsigned long long w) : left(l), right(r), weight(w), accumWeight(0) { }
    weighted_range(T l, T r) : left(l), right(r), weight(1LLU + r - l), accumWeight(0) { }

    bool operator <(const weighted_range<T>& other) const {
      if (left < other.left) return true;
      if (left > other.left) return false;
      if (right < other.right) return true;
      return false;
    }

    bool overlap(const weighted_range<T>& other) const {
      if (left < other.left) return right >= other.left;
      if (left > other.left) return left <= other.right;
      return false;
    }

    T left;
    T right;
    unsigned long long weight;
    unsigned long long accumWeight;
  };

  template<typename T>
  class randomize_base
  {
    public:
      void resetDistribution() { ranges.clear(); }
      void addWeightedRange(T l, T r, unsigned long long w) { addRange(weighted_range<T>(l, r, w)); }
      void addRange(T l, T r) { addRange(weighted_range<T>(l, r)); }
      void range(T l, T r) { resetDistribution(); addRange(weighted_range<T>(l, r)); }

    protected:
      randomize_base() { }

      T nextValue() {
        if (ranges.empty())
          return boost::uniform_int<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max())(rng);
        weighted_range<T> selected = ranges.back();
        if (ranges.size() > 1) {
          unsigned long long r = boost::uniform_int<unsigned long long>(0, selected.accumWeight - 1)(rng);
          for (uint i = 0; i < ranges.size(); i++)
            if (r < ranges[i].accumWeight) {
              selected = ranges[i];
              break;
            }
        }
        return boost::uniform_int<T>(selected.left, selected.right)(rng);
      }

      void addRange(weighted_range<T> wr) {
        for (uint i = 0; i < ranges.size(); i++)
          if (ranges[i].overlap(wr)) throw std::runtime_error("Overlapping range exists.");
        wr.accumWeight = (ranges.empty() ? 0 : ranges.back().accumWeight) + wr.weight;
        ranges.push_back(wr);
      }

      std::vector< weighted_range<T> > ranges;
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
      rand_vec(rand_obj* parent) : __rand_vec<T>() { if (parent != 0) parent->addChild(this); }

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
