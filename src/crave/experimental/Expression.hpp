// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "../frontend/bitsize_traits.hpp"
#include "../ir/UserExpression.hpp"

namespace crave {

template <typename CollectionType>
expression sum(CollectionType const& collection) {
  expression result = value_to_expression(0);
  for (auto& v : collection)
    result = crave::make_expression(result + v());
  return result;
}

template <typename IntegerType>
expression onehot(IntegerType const& x) {
  return crave::make_expression((x > 0) && ((x & (x - 1)) == 0));
}

template <typename IntegerType>
expression bitcount(IntegerType const& x) {
  expression result = value_to_expression(0);
  for (unsigned i = 0; i < bitsize_traits<IntegerType>::value; i++)
    result = crave::make_expression(result + ((x >> i) & 1));
  return result;
}

}  // namespace crave
