#pragma once

#include <sysc/datatypes/int/sc_int.h>
#include <sysc/datatypes/int/sc_uint.h>
#include <sysc/datatypes/bit/sc_bv.h>

#include "bitsize_traits.hpp"

#include <boost/function.hpp>

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
} // namespace crave

//  vim: ft=cpp:ts=2:sw=2:expandtab
