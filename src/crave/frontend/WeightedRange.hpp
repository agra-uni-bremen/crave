// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

namespace crave {

template <typename T>
struct weighted_range {
  weighted_range(T l, T r, unsigned w) : left_(l), right_(r), weight_(w), accumWeight_(0) {}

  bool operator<(const weighted_range<T>& other) const {
    if (left_ < other.left_) return true;
    if (left_ > other.left_) return false;
    if (right_ < other.right_) return true;
    return false;
  }

  bool overlap(const weighted_range<T>& other) const {
    if (left_ < other.left_) return right_ >= other.left_;
    if (left_ > other.left_) return left_ <= other.right_;
    return false;
  }

  T left_;
  T right_;
  unsigned weight_;
  unsigned accumWeight_;
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
