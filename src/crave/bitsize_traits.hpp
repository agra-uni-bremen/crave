#pragma once
#include <boost/mpl/times.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/utility/enable_if.hpp>

namespace crave {

  template <typename Integer>
  struct bitsize_traits
  : boost::mpl::times< 
      boost::mpl::sizeof_<Integer>
    , boost::mpl::int_<8>
  >::type {};

  template<>
  struct bitsize_traits<bool> : boost::mpl::int_<1> {};

  template<typename T>
  struct bitsize_traits< randv<T> > : public bitsize_traits<T> {};
  template<typename T>
  struct bitsize_traits< Variable<T> > : public bitsize_traits<T> {};
  template<typename T>
  struct bitsize_traits< var_tag<T> > : public bitsize_traits<T> {};
  template<typename T>
  struct bitsize_traits< write_ref_tag<T> > : public bitsize_traits<T> {};
  template<typename T>
  struct bitsize_traits< read_ref_tag<T> > : public bitsize_traits<T> {};
  template<typename T>
  struct bitsize_traits< vector_tag<T> > : public bitsize_traits<T> {};
} // namespace crave

namespace boost {
  template<typename T>
  struct is_signed< crave::randv<T> > : public is_signed<T> {};
  template<typename T>
  struct is_signed< crave::Variable<T> > : public is_signed<T> {};
  template<typename T>
  struct is_signed< crave::var_tag<T> > : public is_signed<T> {};
  template<typename T>
  struct is_signed< crave::write_ref_tag<T> > : public is_signed<T> {};
  template<typename T>
  struct is_signed< crave::read_ref_tag<T> > : public is_signed<T> {};
  template<typename T>
  struct is_signed< crave::vector_tag<T> > : public is_signed<T> {};
}

//  vim: ft=cpp:ts=2:sw=2:expandtab
