#include <boost/test/unit_test.hpp>

#include <crave/expression/Evaluator.hpp>

#include <set>
#include <iostream>

//using namespace std;
using namespace crave;

BOOST_FIXTURE_TEST_SUITE(Evaluations_t, Context_Fixture )

BOOST_AUTO_TEST_CASE (logical_not_t1) {
  randv<unsigned int> a(0);
  Evaluator evaluator;

  evaluator.assign(a(), 0u);

  BOOST_REQUIRE( evaluator.evaluate(!(a() != 0)) );
  BOOST_CHECK( evaluator.get_result<bool>() );

  evaluator.assign(a(), 42u);

  BOOST_REQUIRE( evaluator.evaluate(!(a() == 0)) );
  BOOST_CHECK( evaluator.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE (logical_not_t2) {
  randv<unsigned char> a(0);
  randv<unsigned int> b(0);
  Evaluator eval;
  NodePtr expression(eval.get_expression(
    if_then_else(!(a() % 2 == 0), b() > 0 && b() <= 50, b() > 50 && b() <= 100 )
  ));

  eval.assign(a(), 1u);

  BOOST_CHECK( !eval.evaluate(*expression) );

  eval.assign(b(), 35u);

  BOOST_REQUIRE( eval.evaluate(*expression) );
  BOOST_CHECK( eval.get_result<bool>() );

  eval.assign(a(), 2u);
  eval.assign(b(), 75u);

  BOOST_REQUIRE( eval.evaluate(*expression) );
  BOOST_CHECK( eval.get_result<bool>() );

  eval.assign(a(), 1u);

  BOOST_REQUIRE( eval.evaluate(*expression) );
  BOOST_CHECK( !eval.get_result<bool>() );
}


BOOST_AUTO_TEST_CASE (neg_t1) {
  randv<int> a(0);
  randv<int> b(0);
  Evaluator eval;

  eval.assign(a(), 1337);
  BOOST_CHECK(  eval.evaluate(-a() == 1337) );
  BOOST_CHECK( !eval.get_result<bool>() );

  eval.assign(b(), -1337);
  BOOST_CHECK( eval.evaluate(a() == -b()) );
  BOOST_CHECK( eval.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE (neg_t2) {
  int a = 1337;
  randv<int> b(0);
  Evaluator eval;

  eval.assign(b(), -a);
  BOOST_CHECK( eval.evaluate(b()) );
  BOOST_CHECK_EQUAL( eval.get_result<int>(), -1337 );
}

BOOST_AUTO_TEST_CASE (complement_t1) {
  randv<int> a(0);
  randv<int> b(0);
  Evaluator eval;

  eval.assign(a(), 0);
  eval.assign(b(), -1);
  BOOST_CHECK( eval.evaluate(~a() == b()) );
  BOOST_CHECK( eval.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE (complement_t2) {
  int a = 42;
  randv<int> b(0);
  Evaluator eval;

  eval.assign(b(), a);
  BOOST_CHECK( eval.evaluate(~b()) );
  BOOST_CHECK_EQUAL( eval.get_result<int>(), -43 );
}



}


BOOST_AUTO_TEST_SUITE_END() // Evaluations
