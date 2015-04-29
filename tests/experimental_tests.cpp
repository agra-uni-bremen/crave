#define BOOST_TEST_MODULE experimental

#include <crave/experimental/Experimental.hpp>

struct Context_Fixture {
  Context_Fixture() {}
};

#include "test_CoverageSampling.cpp"
#include "test_ObjectHierarchy.cpp"

