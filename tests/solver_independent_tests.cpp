#define BOOST_TEST_MODULE solver_independent
#include <crave/ConstrainedRandom.hpp>

struct Context_Fixture {
  Context_Fixture () {
  }
};

#include "test_Syntax.cpp"
#include "test_Evaluations.cpp"
#include "test_UserConstraint.cpp"
