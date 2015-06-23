#include "../crave/frontend/Distribution.hpp"

namespace crave {
distribution<bool>::distribution(const double prob) : prob_(prob) {}

distribution<bool> distribution<bool>::create(const double prob) {
  distribution<bool> dist(prob);
  return dist;
}

bool distribution<bool>::nextValue() const {
  boost::uniform_01<> dist;
  return dist(rng) <= prob_;
}

std::vector<weighted_range<bool> >& distribution<bool>::ranges() {
  static std::vector<weighted_range<bool> > v;
  return v;
}
}
