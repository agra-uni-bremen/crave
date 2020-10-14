/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2011-2016 University of Bremen, Germany.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/

#include <boost/test/unit_test.hpp>
#include <metaSMT/support/cardinality/Evaluator.hpp>
#include <vector>
#include <metaSMT/support/cardinality.hpp>
#include <metaSMT/frontend/Logic.hpp>

#include <boost/assign/std/vector.hpp>

using namespace metaSMT;
using namespace metaSMT::solver;
using namespace metaSMT::logic;
using namespace metaSMT::cardinality;
using namespace boost::assign;
namespace proto = boost::proto;

BOOST_FIXTURE_TEST_SUITE(cardinality_t, Solver_Fixture )

BOOST_AUTO_TEST_CASE( test_one_hot ) {
  set_option( ctx, "cardinality", "bdd" );

  predicate a = new_variable();
  predicate b = new_variable();
  predicate c = new_variable();
  predicate d = new_variable();
  predicate e = new_variable();
  bool ad, bd, cd, dd, ed;

  std::vector<ContextType::result_type> vec;

  // one predicate
  vec.push_back( evaluate(ctx, a) );
  assumption(ctx, one_hot(ctx, vec) );
  BOOST_REQUIRE( solve(ctx) );
  ad = read_value(ctx, a);
  BOOST_CHECK_EQUAL(ad, true);

  // two predicate
  vec.push_back( evaluate(ctx, b) );
  assumption(ctx, one_hot(ctx, vec) );
  BOOST_REQUIRE( solve(ctx) );
  ad = read_value(ctx, a);
  bd = read_value(ctx, b);
  BOOST_REQUIRE(   (ad == 1 && bd == 0)
                 | (ad == 0 && bd == 1) );

  // five predicates
  vec.push_back( evaluate(ctx, c) );
  vec.push_back( evaluate(ctx, d) );
  vec.push_back( evaluate(ctx, e) );
  assumption(ctx, one_hot(ctx, vec) );
  BOOST_REQUIRE( solve(ctx) );

  ad = read_value(ctx, a);
  bd = read_value(ctx, b);
  cd = read_value(ctx, c);
  dd = read_value(ctx, d);
  ed = read_value(ctx, e);

  BOOST_REQUIRE(   (ad == 1 && bd == 0 && cd == 0 && dd == 0 && ed == 0)
                 | (ad == 0 && bd == 1 && cd == 0 && dd == 0 && ed == 0)
                 | (ad == 0 && bd == 0 && cd == 1 && dd == 0 && ed == 0)
                 | (ad == 0 && bd == 0 && cd == 0 && dd == 1 && ed == 0)
                 | (ad == 0 && bd == 0 && cd == 0 && dd == 0 && ed == 1) );
}

BOOST_AUTO_TEST_CASE( test_cardinality_eq )
{
  set_option( ctx, "cardinality", "bdd" );

  predicate a = new_variable();
  predicate b = new_variable();
  predicate c = new_variable();
  predicate d = new_variable();
  predicate e = new_variable();
  bool ad, bd, cd, dd, ed;

  std::vector<ContextType::result_type> vec;

  // one predicate
  vec.push_back( evaluate(ctx, a) );
  assumption(ctx, cardinality_eq(ctx, vec, 1) );;
  BOOST_REQUIRE( solve(ctx) );
  ad = read_value(ctx, a);
  std::cout << "a = " << ad << std::endl;
  BOOST_CHECK_EQUAL(ad, true);

  // two predicate
  vec.push_back( evaluate(ctx, b) );
  assumption(ctx, cardinality_eq(ctx, vec, 1) );
  BOOST_REQUIRE( solve(ctx) );
  ad = read_value(ctx, a);
  bd = read_value(ctx, b);
  std::cout << "a = " << ad << std::endl;
  std::cout << "b = " << bd << std::endl;
  BOOST_REQUIRE(   (ad == 1 && bd == 0)
                 | (ad == 0 && bd == 1) );

  // five predicates
  vec.push_back( evaluate(ctx, c) );
  vec.push_back( evaluate(ctx, d) );
  vec.push_back( evaluate(ctx, e) );
  assumption(ctx, cardinality_eq(ctx, vec, 1) );
  BOOST_REQUIRE( solve(ctx) );
  ad = read_value(ctx, a);
  bd = read_value(ctx, b);
  cd = read_value(ctx, c);
  dd = read_value(ctx, d);
  ed = read_value(ctx, e);
  std::cout << "a = " << ad << std::endl;
  std::cout << "b = " << bd << std::endl;
  std::cout << "c = " << cd << std::endl;
  std::cout << "d = " << dd << std::endl;
  std::cout << "e = " << ed << std::endl;
  BOOST_REQUIRE(   (ad == 1 && bd == 0 && cd == 0 && dd == 0 && ed == 0)
                 | (ad == 0 && bd == 1 && cd == 0 && dd == 0 && ed == 0)
                 | (ad == 0 && bd == 0 && cd == 1 && dd == 0 && ed == 0)
                 | (ad == 0 && bd == 0 && cd == 0 && dd == 1 && ed == 0)
                 | (ad == 0 && bd == 0 && cd == 0 && dd == 0 && ed == 1) );

  // 
  assumption(ctx, nequal(cardinality_eq(ctx, vec, 1), one_hot(ctx, vec)) );
  BOOST_REQUIRE( !solve(ctx) );
}

template <typename Context, typename Boolean>
void assumeFromUnsigned(Context &ctx, std::vector<Boolean> const &vec, unsigned u) {
  for (unsigned q = 0; q < vec.size(); ++q) {
    if (u & (1 << q)) {
      assumption(ctx, equal(vec[q], True));
    } else {
      assumption(ctx, equal(vec[q], False));
    }
  }
}


template <typename Context>
void checkAllCombinations(Context &ctx) {

  unsigned const width = 6;
  std::vector<predicate> vec;
  for (unsigned u = 0; u < width; ++u) {
    vec.push_back(new_variable());
  }

  for (unsigned u = 0; u < (1 << width); ++u) {
    unsigned count_u = 0;
    for (unsigned v = u; v != 0; v >>= 1) {
      count_u += v & 1;
    }
    for (unsigned r = 0; r <= width; ++r) {
      assumeFromUnsigned(ctx, vec, u);
      assumption(ctx, cardinality_geq(ctx, vec, r));
      BOOST_REQUIRE_EQUAL( solve(ctx), count_u >= r );

      assumeFromUnsigned(ctx, vec, u);
      assumption(ctx, cardinality_leq(ctx, vec, r));
      BOOST_REQUIRE_EQUAL( solve(ctx), count_u <= r );

      assumeFromUnsigned(ctx, vec, u);
      assumption(ctx, cardinality_gt(ctx, vec, r));
      BOOST_REQUIRE_EQUAL( solve(ctx), count_u > r );

      assumeFromUnsigned(ctx, vec, u);
      assumption(ctx, cardinality_lt(ctx, vec, r));
      BOOST_REQUIRE_EQUAL( solve(ctx), count_u < r );

      assumeFromUnsigned(ctx, vec, u);
      assumption(ctx, cardinality_eq(ctx, vec, r));
      BOOST_REQUIRE_EQUAL( solve(ctx), count_u == r );
    }
  }
}

BOOST_AUTO_TEST_CASE( test_bdd_cardinality_with_bv ) {
  set_option( ctx, "cardinality", "bdd" );
  checkAllCombinations(ctx);
}

BOOST_AUTO_TEST_CASE( test_adder_cardinality_with_bv ) {
  set_option( ctx, "cardinality", "adder" );
  checkAllCombinations(ctx);
}

BOOST_AUTO_TEST_SUITE_END() //Solver

//  vim: ft=cpp:ts=2:sw=2:expandtab
