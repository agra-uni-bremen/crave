#pragma once

#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <limits>
#include <vector>
#include <set>
#include <cstdarg>

namespace crave {

  extern boost::mt19937 rng;

  template<typename T>
  struct weighted_range {
    weighted_range(T l, T r, unsigned w) : left(l), right(r), weight(w), accumWeight(0) { }
    weighted_range(T l, T r) : left(l), right(r), weight(r - l + 1), accumWeight(0) { }

    bool operator <(const weighted_range<T>& other) const {
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

  template<typename T>
  struct weighted_value : weighted_range<T>   {
    weighted_value(T v, unsigned w) : weighted_range<T>(v, v, w) { }
    weighted_value(T v) : weighted_range<T>(v, v) { }
  };

  template<typename T>
  struct distribution {
    distribution() : ranges() { }

    distribution& operator()(const weighted_range<T>& range) { 
      addRange(range);
      return *this;
    }

    static distribution create(const weighted_range<T>& range) { 
      distribution dist;
      dist(range);
      return dist;
    }

    void reset() { ranges.clear(); }

    T nextValue() {
      if (ranges.empty())
        return boost::uniform_int<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max())(rng);
      weighted_range<T> selected = ranges.back();
      if (ranges.size() > 1) {
        unsigned r = boost::uniform_int<unsigned>(0, selected.accumWeight - 1)(rng);
        for (uint i = 0; i < ranges.size(); i++)
          if (r < ranges[i].accumWeight) {
            selected = ranges[i];
            break;
          }
      }
      return boost::uniform_int<T>(selected.left, selected.right)(rng);
    }

  private:
    void addRange(weighted_range<T> wr) {
      for (uint i = 0; i < ranges.size(); i++)
        if (ranges[i].overlap(wr)) throw std::runtime_error("Overlapping range exists.");
      wr.accumWeight = (ranges.empty() ? 0 : ranges.back().accumWeight) + wr.weight;
      ranges.push_back(wr);
    }

  private:
    std::vector< weighted_range<T> > ranges;
  };

} // namespace crave

