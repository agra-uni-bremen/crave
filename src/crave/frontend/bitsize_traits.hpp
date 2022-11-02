// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <type_traits>

namespace crave {

/**
 * Define various type traits for CRAVE-defined data types.
 * Type traits are important for the conversion to bit-vector in the backend.
 * When adding new data types, their type traits need to be defined.
 */

template <typename T>
struct randv;
template <typename T>
struct Variable;
template <typename T>
struct WriteReference;
template <typename T>
struct var_tag;
template <typename T>
struct write_ref_tag;
template <typename T>
struct read_ref_tag;
template <typename T>
struct vector_tag;
struct placeholder_tag;

template <typename T, typename Enable = void>
struct bitsize_traits : std::integral_constant<std::size_t, 0> {
  static_assert(sizeof(T) == 0, "zero-sized type");
};

template <typename Integer>
struct bitsize_traits<Integer, typename std::enable_if<std::is_integral<Integer>::value>::type>
    : std::integral_constant<std::size_t, sizeof(Integer) * 8> {};

template <>
struct bitsize_traits<bool> : std::integral_constant<std::size_t, 1> {};

template <>
struct bitsize_traits<placeholder_tag> : public bitsize_traits<unsigned> {};

template <typename T>
struct bitsize_traits<randv<T> > : public bitsize_traits<T> {};

template <typename T>
struct bitsize_traits<Variable<T> > : public bitsize_traits<T> {};

template <typename T>
struct bitsize_traits<WriteReference<T> > : public bitsize_traits<T> {};

template <typename T>
struct bitsize_traits<var_tag<T> > : public bitsize_traits<T> {};

template <typename T>
struct bitsize_traits<write_ref_tag<T> > : public bitsize_traits<T> {};

template <typename T>
struct bitsize_traits<read_ref_tag<T> > : public bitsize_traits<T> {};

template <typename T>
struct bitsize_traits<vector_tag<T> > : public bitsize_traits<T> {};

template <typename Enum>
struct bitsize_traits<Enum, typename std::enable_if<std::is_enum<Enum>::value>::type> : bitsize_traits<int>::type {};

template <typename T>
struct is_crave_variable : std::false_type {};

template <typename T>
struct is_crave_variable<randv<T> > : std::true_type {};

template <typename T>
struct is_crave_variable<write_ref_tag<T> > : std::true_type {};

template <typename T>
struct is_signed : public std::is_signed<T> {};

/**
 * assume bool to be signed (i.e. will be converted to signed bv)
 */
template <>
struct is_signed<bool> : public std::is_signed<int> {};

template <typename T>
struct is_signed<crave::randv<T> > : public crave::is_signed<T> {};

template <typename T>
struct is_signed<crave::Variable<T> > : public crave::is_signed<T> {};

template <typename T>
struct is_signed<crave::var_tag<T> > : public crave::is_signed<T> {};

template <typename T>
struct is_signed<crave::write_ref_tag<T> > : public crave::is_signed<T> {};

template <typename T>
struct is_signed<crave::read_ref_tag<T> > : public crave::is_signed<T> {};

template <typename T>
struct is_signed<crave::vector_tag<T> > : public crave::is_signed<T> {};

template <typename T, typename Enable = void>
struct to_constant_expr {};

template <typename T>
struct is_sysc_dt : std::false_type {};

}  // namespace crave

//  vim: ft=cpp:ts=2:sw=2:expandtab
