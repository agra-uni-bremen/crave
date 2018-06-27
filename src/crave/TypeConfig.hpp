// Copyright 2012-2018 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <type_traits>

#include <boost/multiprecision/cpp_int.hpp>

namespace crave {

template <typename T>
struct is_crave_bigint : std::false_type {};

template <unsigned W>
using unsigned_int = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<W, W, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;

using uint128_t = unsigned_int<128>;
using uint256_t = unsigned_int<256>;
using uint512_t = unsigned_int<512>;
using uint1024_t = unsigned_int<1024>;

template <unsigned W>
struct is_crave_bigint<unsigned_int<W>> : std::true_type {};

namespace detail {
/*
 * Note that boost::multiprecision fixed-width signed cpp_int has an extra sign bit, so the semantics are
 * different from native signed integer type. Signed int types are therefore only defined in namespace detail
 * for now to prevent wrong usage
 */
template <unsigned W>
using signed_int = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<W, W, boost::multiprecision::signed_magnitude, boost::multiprecision::unchecked, void>>;

using int128_t = signed_int<127>;
using int256_t = signed_int<256>;
using int512_t = signed_int<511>;
using int1024_t = signed_int<1023>;

}

template <unsigned W>
struct is_crave_bigint<detail::signed_int<W>> : std::true_type {};

// using underlying_type = uint64_t;
using underlying_type = uint1024_t;

}
