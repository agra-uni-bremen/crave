// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <random>
#include <limits>
#include <vector>
#include <stdexcept>

#include "ConstraintType.hpp"
#include "WeightedRange.hpp"
#include "../RandomSeedManager.hpp"
#include "../TypeConfig.hpp"
#include "../frontend/bitsize_traits.hpp"

namespace crave {

extern RandomSeedManager rng;

template <typename T>
struct distribution_tag;

/*
 * Workaround for the issue that std::uniform_int_distribution is undefined for char,
 * unsigned char and signed char.
 */
template <typename T, typename Generator>
typename std::enable_if<std::is_integral<T>::value, T>::type uniformly_distributed_value(T left, T right, Generator& gen) {
  return std::uniform_int_distribution<T>(left, right)(gen);
}

template <typename Generator>
char uniformly_distributed_value(char left, char right, Generator& gen) {
  return std::uniform_int_distribution<int>(left, right)(gen);
}

template <typename Generator>
signed char uniformly_distributed_value(signed char left, signed char right, Generator& gen) {
  return std::uniform_int_distribution<int>(left, right)(gen);
}

template <typename Generator>
unsigned char uniformly_distributed_value(unsigned char left, unsigned char right, Generator& gen) {
  return std::uniform_int_distribution<int>(left, right)(gen);
}

/**
 * random bigint
 */
namespace detail {

template <unsigned W, typename Generator>
unsigned_int<W> random_big_unsigned(const unsigned_int<W>&max, Generator& gen) {
  if (max <= std::numeric_limits<uint64_t>::max())
    return std::uniform_int_distribution<uint64_t>(0, static_cast<uint64_t>(max))(gen);
  unsigned_int<W> rem = max & std::numeric_limits<uint64_t>::max();
  unsigned_int<W> buckets = (max >> 64) + (rem ? 1 : 0);
  unsigned_int<W> bucket_idx;
  uint64_t random_r;
  do {
    bucket_idx = random_big_unsigned(buckets - 1, gen);
    random_r = std::uniform_int_distribution<uint64_t>()(gen);
    // reject sample if out of range (only possible for the last bucket)
  } while (rem > 0 && bucket_idx + 1 == buckets && random_r > rem);
  return (bucket_idx << 64) + random_r;
}

template <unsigned W, typename Generator>
unsigned_int<W> uniformly_distributed_big_value(const unsigned_int<W>& left, const unsigned_int<W>& right, Generator& gen) {
  assert(left <= right);
  return left + random_big_unsigned(right - left, gen);
}

template <unsigned W, typename Generator>
signed_int<W> uniformly_distributed_big_value(const signed_int<W>& left, const signed_int<W>& right, Generator& gen) {
  assert(left <= right);
  signed_int<W + 1> dist = static_cast<signed_int<W + 1>>(right) - left; // upcast to avoid overflow
  unsigned_int<W + 1> rdist = random_big_unsigned(static_cast<unsigned_int<W + 1>>(dist), gen);
  return static_cast<signed_int<W>>(dist + left - rdist);
}

}

template <typename T, typename Generator>
typename std::enable_if<is_crave_bigint<T>::value && !is_signed<T>::value, T>::type uniformly_distributed_value(const T& left, const T& right, Generator& gen) {
  return detail::uniformly_distributed_big_value<bitsize_traits<T>::value, Generator>(left, right, gen);
}

template <typename T, typename Generator>
typename std::enable_if<is_crave_bigint<T>::value && is_signed<T>::value, T>::type uniformly_distributed_value(const T& left, const T& right, Generator& gen) {
  return detail::uniformly_distributed_big_value<bitsize_traits<T>::value - 1, Generator>(left, right, gen);
}

/*!
 * \ingroup oldAPI
 * \ingroup newAPI
 * \brief Distribution of values to used in constraints.
 * 
 * This class allows you to create a distribution.
 * You can use crave::dist operator to force a variable to be inside a distribution.
 * A distribution is defined by ranges.
 * For details see crave::range and crave::weighted_range.
 */
template <typename T>
struct distribution {
  /*!
   * \brief Defines an empty distribution.
   * 
   * An empty distribution with no ranges corresponds to the whole value range of the base type T.
   */
  distribution() : ranges_() {}

  /*!
   * \brief Defines a distribution with a weighted range
   * 
   * A weighted range uses weights to define which values should occur more often.
   * See crave::weighted_range for details.
   * 
   * \param range weighted range for distribution
   */
  distribution(const weighted_range<T>& range) { addRange(range); }

  /**
   * \brief Adds another range to the distribution.
   * 
   * \param range A weighted range to add to this distribution
   * \return reference to this distribution
   */
  distribution& operator()(const weighted_range<T>& range) {
    addRange(range);
    return *this;
  }

  /**
   * \brief Creates a distribution to use in constraints.
   * 
   * This method creates a distribution_tag<T> which is recognized by the constraint frontend.
   * Typically this is an input argument for crave::dist.
   * 
   * \param range A (weighted) range to create a distribution of.
   * \return distribution_tag<T> for constraint definition
   */
  static distribution_tag<T> create(const weighted_range<T>& range) { return distribution_tag<T>(distribution(range)); }

  /**
   * \brief Creates a simple distribution with given left and right bound.
   * 
   * This method creates a distribution_tag<T> which is recognized by the constraint frontend.
   * It internally creates an equally distributed range from left to right bound.
   * 
   * \param left left bound of the range
   * \param right right bound of the range
   * \return distribution_tag<T> for constraint definition
   */
  static distribution_tag<T> simple_range(T left, T right) {
    return distribution_tag<T>(distribution(range<T>(left, right)));
  }

  /**
   * \brief Deletes all ranges of this distribution
   */
  void reset() { ranges_.clear(); }

  /**
   * \brief Get all ranges of this distribution.
   * \return weighted ranges of this distribution
   */
  std::vector<weighted_range<T> > const& ranges() const { return ranges_; }

  /**
   * \brief Generate next random distribution value.
   * 
   * If no ranges exist for this distribution, an uniform distribution over all possible values of T is used.
   * 
   * \return T Next random value based on specified weights of ranges.
   */
  T nextValue() const {
    if (ranges_.empty()) {
      return uniformly_distributed_value<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), *rng.get());
    }
    weighted_range<T> selected = ranges_.back();
    if (ranges_.size() > 1) {
      underlying_type one = 1;
      underlying_type r = uniformly_distributed_value(one, selected.accumWeight_, *rng.get()) - 1;
      for (unsigned i = 0; i < ranges_.size(); i++)
        if (r < ranges_[i].accumWeight_) {
          selected = ranges_[i];
          break;
        }
    }
    return uniformly_distributed_value<T>(selected.left_, selected.right_, *rng.get());
  }

 protected:
  void addRange(weighted_range<T> wr) {
    for (unsigned i = 0; i < ranges_.size(); i++)
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
    boost::proto::value (*this)(range);
    return *this;
  }
};

}  // namespace crave
