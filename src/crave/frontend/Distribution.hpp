// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <boost/random.hpp>
#include <limits>
#include <vector>
#include "ConstraintType.hpp"
#include "WeightedRange.hpp"
#include "../RandomSeedManager.hpp"

namespace crave {

extern RandomSeedManager rng;

template <typename T>
struct distribution_tag;

template <typename T>
struct distribution {
  distribution() : ranges_() {}

  distribution(const weighted_range<T>& range) { addRange(range); }

  distribution& operator()(const weighted_range<T>& range) {
    addRange(range);
    return *this;
  }

  static distribution_tag<T> create(const weighted_range<T>& range) {
    return distribution_tag<T>(distribution(range));
  }

  static distribution_tag<T> simple_range(T left, T right) {
    return distribution_tag<T>(distribution(range<T>(left, right)));
  }

  void reset() { ranges_.clear(); }

  std::vector<weighted_range<T> > const& ranges() const { return ranges_; }

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

 protected:
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

 protected:
  std::vector<weighted_range<T> > ranges_;
};

template <>
struct distribution<bool> {
  explicit distribution(const double prob = 0.5);

  static distribution_tag<bool> create(const double prob);

  bool nextValue() const;

  std::vector<weighted_range<bool> > const& ranges() const;

 private:
  double prob_;
};

template <typename T>
struct distribution_tag : public Constraint<typename boost::proto::terminal<distribution<T> >::type> {
  typedef Constraint<typename boost::proto::terminal<distribution<T> >::type> base_type;
  explicit distribution_tag(distribution<T> d) : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(d)) {}

  distribution_tag& operator()(const weighted_range<T>& range) {
    boost::proto::value(*this)(range);
    return *this;
  }
};

}  // namespace crave
