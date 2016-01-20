#define BOOST_TEST_MODULE experimental

#include <crave/experimental/Experimental.hpp>
#include <crave/ConstrainedRandom.hpp>

struct Context_Fixture {
  Context_Fixture() {}
};

#include "test_CoverageSampling.cpp"
#include "test_ExperimentalObjectHierarchy.cpp"
#include "test_ExperimentalConstraintManagement.cpp"
#include "test_ExperimentalOperators.cpp"
#include "test_ExperimentalUserConstraint.cpp"
#include "test_ExperimentalContext.cpp"
