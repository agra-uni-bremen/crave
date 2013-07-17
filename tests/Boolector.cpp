#define BOOST_TEST_MODULE Boolector
#include <string>

struct Context_Fixture {
  Context_Fixture () : solver_type("Boolector") { }
  std::string const solver_type;
};

#include "test_Syntax.cpp"
#include "test_Context.cpp"
#include "test_Random_Object.cpp"
#include "test_Constraint_Management.cpp"
#include "test_Vector_Constraint.cpp"
