#pragma once
#include <boost/mpl/times.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>

namespace crave {

  template<typename T> class randv;
  template<typename T> struct Variable;
  template<typename T> struct WriteReference;
  template<typename T> struct var_tag;
  template<typename T> struct write_ref_tag;
  template<typename T> struct read_ref_tag;
  template<typename T> struct vector_tag;

  template <typename T, typename Enable = void>
  struct bitsize_traits : boost::mpl::int_<0> {
    BOOST_STATIC_ASSERT(sizeof(T)==0);
  };

  template <typename Integer>
  struct bitsize_traits<Integer
    , typename boost::enable_if< boost::is_integral<Integer> >::type
  >
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
  struct bitsize_traits< WriteReference<T> > : public bitsize_traits<T> {};
  template<typename T>
  struct bitsize_traits< var_tag<T> > : public bitsize_traits<T> {};
  template<typename T>
  struct bitsize_traits< write_ref_tag<T> > : public bitsize_traits<T> {};
  template<typename T>
  struct bitsize_traits< read_ref_tag<T> > : public bitsize_traits<T> {};
  template<typename T>
  struct bitsize_traits< vector_tag<T> > : public bitsize_traits<T> {};

  template<typename Integral, Integral I>
  struct bitsize_traits< boost::mpl::integral_c<Integral, I> > : public bitsize_traits<Integral> {};

  template <typename Enum>
  struct bitsize_traits<Enum
    , typename boost::enable_if< boost::is_enum<Enum> >::type
  >
  : boost::mpl::sizeof_<int>::type {};

  template <typename T> struct is_crave_variable: boost::mpl::false_ {};
  template <typename T> struct is_crave_variable< randv<T> >: boost::mpl::true_ {};
  template <typename T> struct is_crave_variable< write_ref_tag<T> >: boost::mpl::true_ {};

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
