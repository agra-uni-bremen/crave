#pragma once

#include <sysc/datatypes/int/sc_int.h>
#include <sysc/datatypes/int/sc_uint.h>
#include <sysc/datatypes/bit/sc_bv.h>

#include "../ir/ReferenceExpression.hpp"
#include "bitsize_traits.hpp"
#include "Distribution.hpp"
#include "RandomBase.hpp"

namespace boost {
  template <int N> struct is_signed< sc_dt::sc_bv<N> >: boost::mpl::false_ {};
  template <int N> struct is_signed< sc_dt::sc_int<N> >: boost::mpl::true_ {};
  template <int N> struct is_signed< sc_dt::sc_uint<N> >: boost::mpl::false_ {};
}

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

// XXX last 2 lines are necessary workaround for old compilers such as gcc 4.1.x
#define RANDV_SCDT_INTERFACE(Typename) \
public: \
  void gather_values(std::vector<long>& ch) { ch.insert(ch.end(), this->value.to_long()); } \
  bool next() { static distribution<long> dist; this->value = dist.nextValue(); return true; } \
  randv<Typename>& operator=(unsigned int i) { this->value = i; return *this; } \
  randv<Typename>& operator=(int i) { this->value = i; return *this; } \

#define RANDV_SCDT_REF_EXPR(Typename) \
  template<int N> \
  struct ReferenceExpressionImpl< randv< Typename <N> > > : public ReferenceExpression { \
  public: \
    ReferenceExpressionImpl(randv< Typename <N> > const& value, ReferenceExpression::result_type expr ) : value_(value), expr_(expr) { } \
    virtual ~ReferenceExpressionImpl() { } \
    virtual ReferenceExpression::result_type expr() const { \
      return new EqualOpr(expr_, new Constant(((Typename <N>) value_).to_ulong(), N, crave::is_signed< Typename <N> >::value)); \
    } \
  private: \
    randv< Typename <N> > const & value_; \
    ReferenceExpression::result_type expr_; \
  }; \

  template<int N>
  class randv<sc_dt::sc_bv<N> > : public randv_base<sc_dt::sc_bv<N> >
  {
    typedef sc_dt::sc_bv<N> sc_bv;
    RANDV_COMMON_INTERFACE(sc_bv)
    RANDV_SCDT_INTERFACE(sc_bv)
  };
  RANDV_SCDT_REF_EXPR(sc_dt::sc_bv)

  template<int N>
  class randv<sc_dt::sc_int<N> > : public randv_base<sc_dt::sc_int<N> >
  {
    typedef sc_dt::sc_int<N> sc_int;
    RANDV_COMMON_INTERFACE(sc_int)
    RANDV_SCDT_INTERFACE(sc_int)
    RANDV_ARITHMETIC_INTERFACE(sc_int)
    RANDV_BITWISE_INTERFACE(sc_int)
  };
  RANDV_SCDT_REF_EXPR(sc_dt::sc_int)

  template<int N>
  class randv<sc_dt::sc_uint<N> > : public randv_base<sc_dt::sc_uint<N> >
  {
    typedef sc_dt::sc_uint<N> sc_uint;
    RANDV_COMMON_INTERFACE(sc_uint)
    RANDV_SCDT_INTERFACE(sc_uint)
    RANDV_ARITHMETIC_INTERFACE(sc_uint)
    RANDV_BITWISE_INTERFACE(sc_uint)
  };
  RANDV_SCDT_REF_EXPR(sc_dt::sc_uint)

} // namespace crave

//  vim: ft=cpp:ts=2:sw=2:expandtab
