#pragma once

#include <sysc/datatypes/int/sc_int.h>
#include <sysc/datatypes/int/sc_uint.h>
#include <sysc/datatypes/bit/sc_bv.h>

#include "bitsize_traits.hpp"

namespace crave {

  template<unsigned N>
  struct bitsize_traits< sc_dt::sc_uint<N> > {
    static const unsigned nbits = N;
  };

  template<unsigned N>
  struct bitsize_traits< sc_dt::sc_int<N> > {
    static const unsigned nbits = N;
  };

  template<unsigned N>
  struct bitsize_traits< sc_dt::sc_bv<N> > {
    static const unsigned nbits = N;
  };

} // namespace crave

//  vim: ft=cpp:ts=2:sw=2:expandtab
