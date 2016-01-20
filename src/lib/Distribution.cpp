#include "../crave/frontend/Distribution.hpp"

namespace crave {
distribution<bool>::distribution(const double prob) : prob_(prob) {}

distribution_tag<bool> distribution<bool>::create(const double prob) {
  return distribution_tag<bool>(distribution<bool>(prob));
}

bool distribution<bool>::nextValue() const {
  boost::uniform_01<> dist;
  return dist(*rng.get()) <= prob_;
}

std::vector<weighted_range<bool> > const& distribution<bool>::ranges() const {
  static std::vector<weighted_range<bool> > v;
  return v;
}
}
