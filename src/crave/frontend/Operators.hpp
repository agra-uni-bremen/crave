// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once
#include "../frontend/ReadReference.hpp"
#include "../frontend/WriteReference.hpp"
#include "../frontend/VectorConstraint.hpp"
#include "../frontend/PlaceHolder.hpp"

namespace crave {

template <typename OUT, typename value_type>
OUT& operator<<(OUT& out, var_tag<value_type> const& tag) {
  out << "var<" << tag.id << ">";
  return out;
}

template <typename OUT, typename value_type>
OUT& operator<<(OUT& out, read_ref_tag<value_type> const& tag) {
  out << "read_ref<" << tag.id << ">";
  return out;
}

template <typename OUT, typename value_type>
OUT& operator<<(OUT& out, write_ref_tag<value_type> const& tag) {
  out << "write_ref<" << tag.id << ">";
  return out;
}

template <typename OUT, typename value_type>
OUT& operator<<(OUT& out, vector_tag<value_type> const& tag) {
  out << "vector<" << tag.id << ">";
  return out;
}

template <typename OUT>
OUT& operator<<(OUT& out, placeholder_tag const& tag);

// special operators

struct operator_inside {};

struct operator_dist {};

struct operator_if_then {};

struct operator_if_then_else : operator_if_then {};

struct operator_foreach {};

struct operator_unique {};

struct operator_bitslice {};

boost::proto::terminal<operator_inside>::type const inside = {};
boost::proto::terminal<operator_dist>::type const dist = {};

boost::proto::terminal<operator_if_then>::type const if_then = {};
boost::proto::terminal<operator_if_then_else>::type const if_then_else = {};

boost::proto::terminal<operator_foreach>::type const foreach = {};
boost::proto::terminal<operator_unique>::type const unique = {};

boost::proto::terminal<operator_bitslice>::type const bitslice = {};

template <typename OUT>
OUT& operator<<(OUT& out, operator_inside const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_dist const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_if_then const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_if_then_else const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_foreach const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_unique const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_bitslice const& tag);
}  // namespace crave
