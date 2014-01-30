#pragma once

#include <sysc/datatypes/int/sc_int.h>
#include <sysc/datatypes/int/sc_uint.h>
#include <sysc/datatypes/bit/sc_bv.h>

#include "bitsize_traits.hpp"
#include "RandomBase.hpp"
#include "VectorConstraint.hpp"

namespace crave {
  template <typename T, int N=0> struct is_sysc_dt: boost::mpl::false_ {};
  template <int N> struct is_sysc_dt< sc_dt::sc_bv<N> >: boost::mpl::true_ {};
  template <int N> struct is_sysc_dt< sc_dt::sc_int<N> >: boost::mpl::true_ {};
  template <int N> struct is_sysc_dt< sc_dt::sc_uint<N> >: boost::mpl::true_ {};

  template <class SCDT> struct sc_dt_width {};
  template <template<int> class SCDT, int N> struct sc_dt_width< SCDT<N> > : boost::mpl::int_<N> {};

  template<typename T>
  struct bitsize_traits< T, typename boost::enable_if< is_sysc_dt<T> >::type >
  : sc_dt_width<T>
  {};

  template<int N>
  class randv_prim_base<sc_dt::sc_bv<N> > : public rand_base
  {
      typedef sc_dt::sc_bv<N> sc_bv;
    public:
      operator sc_bv() const { return value; }
      friend ostream& operator<<(ostream& os, const randv_prim_base<sc_bv>& e) { os << e.value; return os; }
      WriteReference<sc_bv> const& operator()() const { return var; }
      CppType type() { return UNSUPPORTED; }
      virtual void gatherValues(std::vector<long>& ch) { ch.insert(ch.end(), value.to_long()); }
      virtual void gatherValues(std::vector<unsigned long>& ch) { ch.insert(ch.end(), value.to_ulong()); }
      virtual std::size_t numValues() const { return 1; }

    protected:
      randv_prim_base(rand_obj_base* parent) : var(value) { if (parent != 0) parent->addChild(this, true); }
      randv_prim_base(const randv_prim_base& other) : var(value), value(other.value) { }
      WriteReference<sc_bv> var;
      sc_bv value;
  };

  template<int N>
  class rand_vec<sc_dt::sc_bv<N> > : public __rand_vec<sc_dt::sc_bv<N> >, public rand_base
  {
      typedef sc_dt::sc_bv<N> sc_bv;
    public:
      rand_vec(rand_obj_base* parent) : __rand_vec<sc_bv>() { if (parent != 0) parent->addChild(this, true); }

      virtual bool next() {
        static randv<unsigned> default_size(NULL);
        default_size.range(5, 10);
        default_size.next();
        static randv<sc_bv> r(NULL);
        this->clear();
        for (uint i = 0; i < default_size; i++) {
            r.next();
            this->push_back(r);
        }
        return true;
      }
      virtual void gatherValues(std::vector<long>& ch) { }
      virtual void gatherValues(std::vector<unsigned long>& ch) { }
      virtual std::size_t numValues() {
         return 0;
      }
  };

} // namespace crave

//  vim: ft=cpp:ts=2:sw=2:expandtab
