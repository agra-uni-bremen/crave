#include <boost/test/unit_test.hpp>

#include <crave/utils/Evaluator.hpp>

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

BOOST_AUTO_TEST_CASE (logical_and_t1) {
  randv<bool> a(0);
  randv<bool> b(0);
  randv<bool> c(0);
  Evaluator eval;

  eval.assign(a(), true);
  eval.assign(b(), true);

  BOOST_CHECK( eval.evaluate(a() && b()) );
  BOOST_CHECK( eval.get_result<bool>() );

  eval.assign(c(), false);

  BOOST_CHECK(  eval.evaluate(c() == (a() && b())) );
  BOOST_CHECK( !eval.get_result<bool>() );

  eval.assign(a(), false);
  eval.assign(b(), false);

  BOOST_CHECK( eval.evaluate(c() == (a() && b())) );
  BOOST_CHECK( eval.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE (logical_or_t1) {
  randv<bool> a(0);
  randv<bool> b(0);
  randv<bool> c(0);
  Evaluator eval;

  eval.assign(a(), false);
  eval.assign(b(), false);

  BOOST_CHECK(  eval.evaluate(a() || b()) );
  BOOST_CHECK( !eval.get_result<bool>() );

  eval.assign(c(), false);

  BOOST_CHECK( eval.evaluate(c() == (a() || b())) );
  BOOST_CHECK( eval.get_result<bool>() );

  eval.assign(a(), true);
  eval.assign(c(), true);

  BOOST_CHECK( eval.evaluate(c() == (a() || b())) );
  BOOST_CHECK( eval.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE (equal_t1) {
  randv<unsigned int> a(0);
  randv<unsigned int> b(0);
  Evaluator eval;

  eval.assign(a(), 65535);

  BOOST_REQUIRE( eval.evaluate(a()) );
  BOOST_CHECK_EQUAL( eval.get_result<unsigned int>(), 65535 );
  BOOST_REQUIRE( eval.evaluate(a() == 65535) );
  BOOST_CHECK( eval.get_result<bool>() );

  eval.assign(b(), 5);

  BOOST_REQUIRE( eval.evaluate(a() == b()));
  BOOST_CHECK( !eval.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE (not_equal_t1) {
  randv<unsigned int> a(0);
  randv<unsigned int> b(0);
  Evaluator eval;

  eval.assign(a(), 25u);

  for (unsigned int i = 0; i < 50u; ++i) {

    eval.assign(b(), i);
    BOOST_REQUIRE( eval.evaluate(a() != b()) );

    if (i != 25u)
      BOOST_CHECK( eval.get_result<bool>() );
    else
      BOOST_CHECK( !eval.get_result<bool>() );
  }
}

BOOST_AUTO_TEST_CASE( less )
{
  Variable<unsigned> a;
  randv<unsigned> b(0);
  Evaluator eval;

  for (unsigned int i = 0u; i < 50u; ++i) {
    eval.assign(a, i);
    BOOST_CHECK( eval.evaluate(a < 50u) );
    BOOST_CHECK( eval.get_result<bool>() );

    eval.assign(b(), i);
    BOOST_CHECK( eval.evaluate(b() < 50u) );
    BOOST_CHECK( eval.get_result<bool>() );
  }
}

BOOST_AUTO_TEST_CASE( less_equal )
{
  Variable<unsigned> a;
  randv<unsigned> b(0);
  Evaluator eval;

  for (unsigned int i = 0u; i <= 50u; ++i) {
    eval.assign(a, i);
    BOOST_CHECK( eval.evaluate(a <= 50u) );
    BOOST_CHECK( eval.get_result<bool>() );

    eval.assign(b(), i);
    BOOST_CHECK( eval.evaluate(b() <= 50u) );
    BOOST_CHECK( eval.get_result<bool>() );
  }
}

BOOST_AUTO_TEST_CASE( greater )
{
  Variable<unsigned> a;
  randv<unsigned> b(0);
  Evaluator eval;

  for (int i = 50; i > 0; --i) {
    eval.assign(a, i);
    BOOST_CHECK( eval.evaluate(a > 0u) );
    BOOST_CHECK( eval.get_result<bool>() );

    eval.assign(b(), i);
    BOOST_CHECK( eval.evaluate(b() > 0u) );
    BOOST_CHECK( eval.get_result<bool>() );
  }
}

BOOST_AUTO_TEST_CASE( greater_equal )
{
  Variable<unsigned> a;
  randv<unsigned> b(0);
  Evaluator eval;

  for (int i = 50; i >= 0; --i) {
    eval.assign(a, i);
    BOOST_CHECK( eval.evaluate(a >= 0u) );
    BOOST_CHECK( eval.get_result<bool>() );

    eval.assign(b(), i);
    BOOST_CHECK( eval.evaluate(b() >= 0u) );
    BOOST_CHECK( eval.get_result<bool>() );
  }
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

BOOST_AUTO_TEST_CASE (bitwise_and_t1) {
  randv<unsigned int> a(0);
  randv<unsigned int> b(0);
  Evaluator eval;

  eval.assign(a(), 42);
  eval.assign(b(), 1337);

  BOOST_REQUIRE( eval.evaluate(a() & b()) );
  BOOST_CHECK_EQUAL( eval.get_result<unsigned int>(), 40 );
}

BOOST_AUTO_TEST_CASE (bitwise_or_t1) {
  randv<unsigned int> a(0);
  randv<unsigned int> b(0);
  Evaluator eval;

  eval.assign(a(), 42);
  eval.assign(b(), 1337);

  BOOST_REQUIRE( eval.evaluate(a() | b()) );
  BOOST_CHECK_EQUAL( eval.get_result<unsigned int>(), 1339 );
}

BOOST_AUTO_TEST_CASE (xor_t1) {
  randv<bool> a(0);
  randv<bool> b(0);
  Evaluator eval;

  eval.assign(a(), false);
  eval.assign(b(), false);

  BOOST_REQUIRE( eval.evaluate(a() ^ b()) );
  BOOST_CHECK_EQUAL( eval.get_result<bool>(), false );

  eval.assign(b(), true);

  BOOST_REQUIRE( eval.evaluate(a() ^ b()) );
  BOOST_CHECK_EQUAL( eval.get_result<bool>(), true );
}

BOOST_AUTO_TEST_CASE (xor_t2) {
  randv<unsigned int> a(0);
  randv<unsigned int> b(0);
  Evaluator eval;

  eval.assign(a(), 65535);
  eval.assign(b(), 4080);

  BOOST_REQUIRE( eval.evaluate(a() ^ b()) );
  BOOST_CHECK_EQUAL( eval.get_result<unsigned int>(), 61455 );
}

BOOST_AUTO_TEST_CASE ( shiftleft )
{
  Variable<unsigned> a;
  Variable<char> b;
  Evaluator eval;

  int count = 0;
  while( ++count < 256) {

    eval.assign(a, count);
    eval.assign(b, count % (sizeof(unsigned) << 3u));

    BOOST_CHECK( eval.evaluate(a << b) );
    BOOST_CHECK_EQUAL( eval.get_result<unsigned>(), count << (count % (sizeof(unsigned) << 3u)) );
  }
}

BOOST_AUTO_TEST_CASE ( shiftright )
{
  Variable<unsigned> a;
  Variable<char> b;
  Evaluator eval;

  int count = 0;
  while ( 256 > ++count ) {

    eval.assign(a, count + 256);
    eval.assign(b, count % 8);

    BOOST_CHECK( eval.evaluate(a >> b) );
    BOOST_CHECK_EQUAL( eval.get_result<unsigned>(), (count + 256) >> (count % 8) );
  }
}

BOOST_AUTO_TEST_CASE( plus_minus )
{
  Variable<unsigned int> a;
  Variable<unsigned int> b;
  Evaluator eval;

  unsigned int cnt = 0;
  while ( cnt++ < 300 ) {

    eval.assign(a, cnt * cnt);
    eval.assign(b, cnt + cnt);

    BOOST_CHECK( eval.evaluate(a + b) );
    BOOST_CHECK_EQUAL( eval.get_result<unsigned>(), (cnt * cnt) + (cnt + cnt) );

    BOOST_CHECK( eval.evaluate(a - b) );
    BOOST_CHECK_EQUAL( eval.get_result<unsigned>(), (cnt * cnt) - (cnt + cnt) );
  }
}

BOOST_AUTO_TEST_CASE( mult_mod )
{
  randv<int> a(0);
  randv<int> b(0);
  Evaluator eval;

  for (int i = -3; i <= 3; i++) {
    for (int j = -3; j <= 3; j++) {

      eval.assign(a(), i);
      eval.assign(b(), j);

      BOOST_CHECK( eval.evaluate(a() * b() % 6) );
      BOOST_CHECK_EQUAL( eval.get_result<int>(), i * j % 6 );
    }
  }

  eval.assign(b(), 0);

  BOOST_CHECK( !eval.evaluate(a() % b()) );
}

BOOST_AUTO_TEST_CASE( divide )
{
  Variable<short> a;
  Variable<short> b;
  Evaluator eval;

  unsigned int cnt = 1;
  while ( cnt++ < 256 ) {

    eval.assign(a, cnt * cnt);
    eval.assign(b, cnt + cnt);

    BOOST_CHECK( eval.evaluate(a / b) );
    BOOST_CHECK_EQUAL( eval.get_result<short>(), (cnt * cnt) / (cnt + cnt) );

    BOOST_CHECK( eval.evaluate(a % b) );
    BOOST_CHECK_EQUAL( eval.get_result<short>(), (cnt * cnt) % (cnt + cnt) );
  }

  eval.assign(b, 0u);
  BOOST_CHECK( !eval.evaluate(a / b) );
}

BOOST_AUTO_TEST_CASE ( element_inside_set )
{
  std::set<unsigned> s;
  s.insert(1);
  s.insert(7);
  s.insert(9);

  randv<unsigned> x(0);
  Evaluator eval;

  eval.assign(x(), 1);

  BOOST_CHECK( eval.evaluate(inside(x(), s)) );
  BOOST_CHECK( eval.get_result<bool>() );

  eval.assign(x(), 5);

  BOOST_CHECK(  eval.evaluate(inside(x(), s)) );
  BOOST_CHECK( !eval.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE ( element_inside_vec )
{
  std::vector<unsigned> v;
  v.push_back(1);
  v.push_back(7);
  v.push_back(9);

  randv<unsigned> x(0);
  Evaluator eval;

  eval.assign(x(), 7u);

  BOOST_CHECK( eval.evaluate(inside(x(), v)) );
  BOOST_CHECK( eval.get_result<bool>() );

  eval.assign(x(), 5u);

  BOOST_CHECK(  eval.evaluate(inside(x(), v)) );
  BOOST_CHECK( !eval.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE ( element_inside_array )
{
  unsigned a[3];
  a[0] = 1;
  a[1] = 7;
  a[2] = 9;

  randv<unsigned> x(0);
  Evaluator eval;

  eval.assign(x(), 9);

  BOOST_CHECK( eval.evaluate(inside(x(), a)) );
  BOOST_CHECK( eval.get_result<bool>() );

  eval.assign(x(), 5u);

  BOOST_CHECK(  eval.evaluate(inside(x(), a)) );
  BOOST_CHECK( !eval.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE ( element_inside_list )
{
  std::list<unsigned> l;
  l.push_back(1);
  l.push_back(7);
  l.push_back(9);

  randv<unsigned> x(0);
  Evaluator eval;

  eval.assign(x(), 7u);

  BOOST_CHECK( eval.evaluate(inside(x(), l)) );
  BOOST_CHECK( eval.get_result<bool>() );

  eval.assign(x(), 5u);

  BOOST_CHECK(  eval.evaluate(inside(x(), l)) );
  BOOST_CHECK( !eval.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE ( element_not_inside )
{
  Evaluator eval;
  {
    std::set<unsigned> s;
    randv<unsigned> x(0);
    eval.assign(x(), 1u);

    BOOST_REQUIRE(  eval.evaluate(inside(x(), s)) );
    BOOST_REQUIRE( !eval.get_result<bool>() );

    s.insert(1u);

    BOOST_REQUIRE( eval.evaluate(inside(x(), s)) );
    BOOST_REQUIRE( eval.get_result<bool>() );
  } {
    std::vector<unsigned> v;

    randv<unsigned> x(0);
    eval.assign(x(), 1u);

    BOOST_REQUIRE(  eval.evaluate(inside(x(), v)) );
    BOOST_REQUIRE( !eval.get_result<bool>() );

    v.push_back(1u);

    BOOST_REQUIRE( eval.evaluate(inside(x(), v)) );
    BOOST_REQUIRE( eval.get_result<bool>() );
  } {
    std::vector<unsigned> l;
    randv<unsigned> x(0);
    eval.assign(x(), 1u);

    BOOST_REQUIRE(  eval.evaluate(inside(x(), l)) );
    BOOST_REQUIRE( !eval.get_result<bool>() );

    l.push_back(1u);

    BOOST_REQUIRE( eval.evaluate(inside(x(), l)) );
    BOOST_REQUIRE( eval.get_result<bool>() );
  }
}

BOOST_AUTO_TEST_CASE (if_then_else_t1) {
  Variable<unsigned int> a;
  randv<unsigned int> b(0);
  Evaluator eval;
  NodePtr expr(eval.get_expression(if_then_else(a < 5,
                                                b() > 0 && b() <= 50,
                                                b() > 50 && b() <= 100 )));

  for (int i = 0; i < 10; ++i) {

    eval.assign(a, i);
    eval.assign(b(), 25);
    BOOST_REQUIRE( eval.evaluate(*expr) );

    if (i < 5) {
      BOOST_REQUIRE(  eval.get_result<bool>() );
    } else {
      BOOST_REQUIRE( !eval.get_result<bool>() );
    }
    eval.assign(b(), 75);
    BOOST_REQUIRE( eval.evaluate(*expr) );

    if (i < 5) {
      BOOST_REQUIRE( !eval.get_result<bool>() );
    } else {
      BOOST_REQUIRE(  eval.get_result<bool>() );
    }
  }
}

BOOST_AUTO_TEST_CASE (if_then_t1) {
  Variable<unsigned int> a;
  randv<unsigned int> b(0);
  Evaluator eval;
  NodePtr expr(eval.get_expression(if_then(a < 5, b() > 0 && b() <= 100 )));

  for (int i = 0; i < 10; ++i) {

    eval.assign(a, i);
    eval.assign(b(), 25u);
    BOOST_REQUIRE( eval.evaluate(*expr) );
    BOOST_REQUIRE( eval.get_result<bool>() );

    eval.assign(b(), 705u);
    BOOST_REQUIRE( eval.evaluate(*expr) );

    if (i < 5) {
      BOOST_REQUIRE( !eval.get_result<bool>() );
    } else {
      BOOST_REQUIRE(  eval.get_result<bool>() );
    }
  }
}

BOOST_AUTO_TEST_CASE (equal_t2) {
  randv<unsigned int> a(0);
  randv<unsigned int> b(0);
  Evaluator eval;

  eval.assign(a(), 1);
  eval.assign(b(), 2);

  BOOST_REQUIRE( eval.evaluate(a() == b()));
  BOOST_CHECK( !eval.get_result<bool>() );
}

BOOST_AUTO_TEST_CASE (equal_t3) {
  randv<unsigned int> a(0);
  randv<unsigned int> b(0);
  randv<unsigned int> c(0);
  Evaluator eval;

  eval.assign(a(), 1);
  eval.assign(b(), 2);
  eval.assign(c(), 3);

  BOOST_REQUIRE( eval.evaluate(a() + b() == c() ));
  BOOST_CHECK( eval.get_result<bool>() );
}


BOOST_AUTO_TEST_SUITE_END() // Evaluations
