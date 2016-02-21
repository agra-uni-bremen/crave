#include <boost/test/unit_test.hpp>

// using namespace std;
using namespace crave;

BOOST_FIXTURE_TEST_SUITE(Syntax, Context_Fixture)

BOOST_AUTO_TEST_CASE(constants) {
  //  Generator<> gen;
  Variable<unsigned> x;
  check(x < 10);
  check(x == 100);
  check(x >= 3);
  check((x != 510, x == 510));
  check((x != 510 && x == 510));
}

BOOST_AUTO_TEST_SUITE_END()  // Syntax

//  vim: ft=cpp:ts=2:sw=2:expandtab
