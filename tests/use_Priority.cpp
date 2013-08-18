#define BOOST_TEST_MODULE Priority
#include <crave/ConstrainedRandom.hpp>

#include <string>

struct Context_Fixture {
  Context_Fixture () {
    crave::set_global_seed(0);
    crave::set_solver_backend("Priority");
  }
};

#include "test_Syntax.cpp"
#include "test_Context.cpp"
#include "test_Operators.cpp"
#include "test_Random_Object.cpp"
#include "test_Constraint_Management.cpp"
#include "test_Vector_Constraint.cpp"
#include "test_Distribution.cpp"
