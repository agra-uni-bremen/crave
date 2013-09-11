#include <boost/test/unit_test.hpp>

#include <crave/expression/EvalVisitor.hpp>

#include <set>
#include <iostream>

//using namespace std;
using namespace crave;

BOOST_FIXTURE_TEST_SUITE(Evaluations_t, Context_Fixture )

BOOST_AUTO_TEST_CASE (logical_not_t1) {
  randv<unsigned int> a(0);
  EvalVisitor::eval_map assignments;

  assignments[a().id()] = Constant(0, 32, false);
  EvalVisitor evaluator(assignments);

  BOOST_REQUIRE( evaluator.evaluate(!(a() != 0)) );
  BOOST_CHECK( evaluator.get_result().value() );

  assignments[a().id()] = Constant(42, 32, false);

  BOOST_REQUIRE( evaluator.evaluate(!(a() == 0), assignments) );
  BOOST_CHECK( evaluator.get_result().value() );
}

BOOST_AUTO_TEST_CASE (logical_not_t2) {
  randv<unsigned char> a(0);
  randv<unsigned int> b(0);
  EvalVisitor::eval_map assignments;
  EvalVisitor eval;

  assignments[a().id()] = Constant(1, 8, false);
  assignments[b().id()] = Constant(35, 32, false);
  eval.set_evaluation_map(assignments);

  NodePtr expression(eval.get_expression(
    if_then_else(!(a() % 2 == 0), b() > 0 && b() <= 50, b() > 50 && b() <= 100 )
  ));
  BOOST_REQUIRE( eval.evaluate(*expression) );
  BOOST_CHECK( eval.get_result().value() );

  assignments[a().id()] = Constant(2, 8, false);
  assignments[b().id()] = Constant(75, 32, false);

  BOOST_REQUIRE( eval.evaluate(*expression, assignments) );
  BOOST_CHECK( eval.get_result().value() );

  assignments[a().id()] = Constant(1, 8, false);

  BOOST_REQUIRE( eval.evaluate(*expression, assignments) );
  
  std::cout << "val = " << eval.get_result().value() << std::endl;
  
  BOOST_CHECK( !eval.get_result().value() );

  assignments.erase(b().id());

  eval.set_evaluation_map(assignments);
  BOOST_CHECK( !eval.evaluate(*expression) );
}


BOOST_AUTO_TEST_SUITE_END() // Evaluations
