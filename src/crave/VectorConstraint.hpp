#pragma once

#include "Constraint.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <cassert>

namespace crave {

  class __rand_vec_base { };

  static std::map<int, __rand_vec_base*> vectorBaseMap;

  template<typename T1, typename T2>
  class __rand_vec_base1 : public __rand_vec_base
  {
    public:
      __rand_vec_base1() { vectorBaseMap[sym_vec.id()] = this; }
      const Vector<T1>& operator()() const { return sym_vec; }

      typename std::vector<T2>::size_type size() const { return real_vec.size(); }
      T1& operator[](const int& idx) const { return (T1&) real_vec[idx]; }
      void resize(typename std::vector<T2>::size_type n) { real_vec.resize(n); }
      void push_back(const T1& x) { real_vec.push_back(x); }
      void clear() { real_vec.clear(); }

      void print() {
        std::cout << "vector " << sym_vec.id() << ": ";
        for (uint i = 0; i < real_vec.size(); i++)
          std::cout << real_vec[i] << ", ";
        std::cout << std::endl;
      }

    protected:
      Vector<T1> sym_vec;
      std::vector<T2> real_vec;
  };

  template<typename T>
  class __rand_vec : public __rand_vec_base1<T, T> { };

  template<>
  class __rand_vec<bool> : public __rand_vec_base1<bool, char> { };

  struct vecIdx {
    int val;
    int bound; // 0 = free

    vecIdx(int v, int b) : val(v), bound(b) { }

    bool operator<(const vecIdx& other) const {
      if (val != other.val) return val < other.val;
      return bound < other.bound;
    }

    vecIdx operator+(const vecIdx& other) const {
      assert(bound == 0 || other.bound == 0);
      return vecIdx(val + other.val, bound + other.bound);
    }

    vecIdx operator-(const vecIdx& other) const {
      assert(other.bound == 0 || other.bound == bound);
      return vecIdx(val - other.val, bound - other.bound);
    }
  };

  template <typename OUT>
  OUT & operator<< (OUT & out, vecIdx const & idx ) {
    out << "[";
    if (idx.bound != 0) out << "$" << idx.bound << "$";
    if (idx.val < 0) out << "-" << (-idx.val); else out << "+" << idx.val;
    out << "]";
    return out;
  }

  struct vecVar {
    int vecId;
    vecIdx index;

    vecVar(int id) : vecId(id), index(0, 0) { }
    vecVar(int id, vecIdx idx) : vecId(id), index(idx.val, idx.bound) { }

    bool operator<(const vecVar& other) const {
      if (vecId != other.vecId) return vecId < other.vecId;
      return index < other.index;
    }
  };

  template <typename OUT>
  OUT & operator<< (OUT & out, vecVar const & vv ) {
    out << "vector<" << vv.vecId << ">" << vv.index;
    return out;
  }

};
