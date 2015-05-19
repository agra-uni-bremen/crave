// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

namespace crave {

template <typename T>
struct weighted_range {
  weighted_range(T l, T r, unsigned w) :
  left(l), right(r), weight(w), accumWeight(0) {}

  bool operator<(const weighted_range<T>& other) const {
    if (left < other.left) return true;
    if (left > other.left) return false;
    if (right < other.right) return true;
    return false;
  }

  bool overlap(const weighted_range<T>& other) const {
    if (left < other.left) return right >= other.left;
    if (left > other.left) return left <= other.right;
    return false;
  }

  T left;
  T right;
  unsigned weight;
  unsigned accumWeight;
};

template <typename T>
struct range : weighted_range<T> {
  range(T l, T r) : weighted_range<T>(l, r, 1u + r - l) {}
};

template <typename T>
struct weighted_value : weighted_range<T> {
  weighted_value(T v, unsigned w) : weighted_range<T>(v, v, w) {}
};
}  // namespace crave
