#define BOOST_TEST_MODULE Z3
#include <string>

struct Context_Fixture {
  Context_Fixture () : solver_type("Z3") { }
  std::string const solver_type;
};

#include "test_Syntax.cpp"
#include "test_Context.cpp"
#include "test_Random_Object.cpp"
#include "test_Constraint_Management.cpp"
#include "test_Vector_Constraint.cpp"
#include "test_Distribution.cpp"
