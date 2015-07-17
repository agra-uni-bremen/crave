// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <boost/random.hpp>
#include <limits>
#include <vector>
#include "../ir/Node.hpp"
#include "WeightedRange.hpp"
#include "../RandomSeedManager.hpp"

namespace crave {

extern RandomSeedManager rng;

template <typename T>
struct distribution : Node {
  distribution() : ranges_() {}

  distribution& operator()(const weighted_range<T>& range) {
    addRange(range);
    return *this;
  }

  static distribution create(const weighted_range<T>& range) {
    distribution dist;
    dist(range);
    return dist;
  }

  static distribution simple_range(T left, T right) {
    distribution dist;
    dist(range<T>(left, right));
    return dist;
  }

  void reset() { ranges_.clear(); }

  std::vector<weighted_range<T> >& ranges() { return ranges_; }

  T nextValue() const {
    if (ranges_.empty()) {
      return boost::uniform_int<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max())(*rng.get());
    }
    weighted_range<T> selected = ranges_.back();
    if (ranges_.size() > 1) {
      unsigned r = boost::uniform_int<unsigned>(0, selected.accumWeight_ - 1)(*rng.get());
      for (uint i = 0; i < ranges_.size(); i++)
        if (r < ranges_[i].accumWeight_) {
          selected = ranges_[i];
          break;
        }
    }
    return boost::uniform_int<T>(selected.left_, selected.right_)(*rng.get());
  }

 private:
  void addRange(weighted_range<T> wr) {
    for (uint i = 0; i < ranges_.size(); i++)
      if (ranges_[i].overlap(wr)) {
        throw std::runtime_error("Overlapping range exists.");
      }
    if (ranges_.empty()) {
      wr.accumWeight_ = wr.weight_;
    } else {
      wr.accumWeight_ = ranges_.back().accumWeight_ + wr.weight_;
    }
    ranges_.push_back(wr);
  }

 private:
  std::vector<weighted_range<T> > ranges_;
};

template <>
struct distribution<bool> : Node {
  explicit distribution(const double prob = 0.5);

  static distribution create(const double prob);

  bool nextValue() const;

  std::vector<weighted_range<bool> >& ranges();

 private:
  double prob_;
};

}  // namespace crave
