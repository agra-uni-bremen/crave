#pragma once

#include "Constraint.hpp"

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>

namespace platzhalter {

  enum CppType { 
      UNSUPPORTED
    , BOOL
    , INT
    , UINT 
    , CHAR
    , UCHAR
    , SHORT
    , USHORT
    , LONG
    , ULONG
    , LLONG
    , ULLONG
  };

  template<typename T>
  class TypeInfo {
    public:
      operator CppType() const { return UNSUPPORTED; }  
  };

#define _DEF_TYPE(typename, cpptype) \
template<> \
class TypeInfo<typename> \
{ \
  public: \
    operator CppType() const { return cpptype; } \
}; \

  _DEF_TYPE(bool, BOOL)
  _DEF_TYPE(int, INT)
  _DEF_TYPE(unsigned int, UINT)
  _DEF_TYPE(char, CHAR)
  _DEF_TYPE(unsigned char, UCHAR)
  _DEF_TYPE(short, SHORT)
  _DEF_TYPE(unsigned short, USHORT)
  _DEF_TYPE(long, LONG)
  _DEF_TYPE(unsigned long, ULONG)
  _DEF_TYPE(long long, LLONG)
  _DEF_TYPE(unsigned long long, ULLONG)

#undef  _DEF_TYPE

  typedef Variable<unsigned int> IndexVariable;
  static const IndexVariable _i;

  class __rand_vec_base {
    public:
      virtual CppType element_type() { }
  };

  typedef std::map<int, __rand_vec_base*> RandVecMap;
  extern RandVecMap __rand_vec_map;

  template<typename T>
  class __rand_vec : public __rand_vec_base
  {
    public:
      __rand_vec() { __rand_vec_map[sym_vec.id()] = this; }
      const Vector<T>& operator()() const { return sym_vec; }

      CppType element_type() { return TypeInfo<T>(); }

      int size() const { return real_vec.size(); }
      const T& operator[](const int& idx) const { return real_vec[idx]; } 
      void push_back(const T& x) { real_vec.push_back(x); }
      void clear() { real_vec.clear(); }

      void print() {
        std::cout << "vector " << sym_vec.id() << ": ";
        for (uint i = 0; i < real_vec.size(); i++)    
          std::cout << real_vec[i] << ", ";
        std::cout << std::endl;
      }

    protected:
      Vector<T> sym_vec;
      std::vector<T> real_vec;
  };

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
