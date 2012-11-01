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



  template<typename T> struct AssignResult;
  extern boost::function0<bool> random_bit;

  template<typename T>
  inline void _AssignSCDT(T & value, std::string const & s)
  {
    std::string::const_reverse_iterator it = s.rbegin();
    for (unsigned i = 0; i < s.size(); i++) {
      switch (*it) {
        case '0':
          value[i] = false;
          break;
        case '1':
          value[i] = true;
          break;
        default:
          value[i] = random_bit();
      }
      ++it;
    }
  }

  template <int N>
  struct AssignResult<sc_dt::sc_uint<N> > {
    void operator() (sc_dt::sc_uint<N> & value, std::string const & s)
    {
      _AssignSCDT(value, s);
    }
  };


  template <int N>
  struct AssignResult<sc_dt::sc_int<N> > {
    void operator() (sc_dt::sc_int<N> & value, std::string const & s)
    {
      _AssignSCDT(value, s);
    }
  };

  template <int N>
  struct AssignResult<sc_dt::sc_bv<N> > {
    void operator() (sc_dt::sc_bv<N> & value, std::string const & s)
    {
      _AssignSCDT(value, s);
    }
  };

} // namespace crave

//  vim: ft=cpp:ts=2:sw=2:expandtab
