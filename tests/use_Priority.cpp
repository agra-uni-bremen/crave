#define BOOST_TEST_MODULE Priority
#include <crave/ConstrainedRandom.hpp>

#include <string>

struct Context_Fixture {
  Context_Fixture () {
    crave::init("Priority", "./crave.cfg");
  }
};

#include "test_Syntax.cpp"
#include "test_Context.cpp"
#include "test_Operators.cpp"
#include "test_Random_Object.cpp"
#include "test_Constraint_Management.cpp"
#include "test_Vector_Constraint.cpp"
#include "test_Distribution.cpp"
#include "test_Evaluations.cpp"
