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

#include <metaSMT/frontend/QF_UF.hpp>
#include <metaSMT/frontend/QF_BV.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/dynamic_bitset.hpp>
#include <string>

using namespace metaSMT;
using namespace metaSMT::logic;
using namespace metaSMT::logic::QF_UF;
using namespace metaSMT::logic::QF_BV;
namespace proto = boost::proto;
using boost::dynamic_bitset;


BOOST_FIXTURE_TEST_SUITE(QF_UF, Solver_Fixture )

BOOST_AUTO_TEST_CASE( equal_bool ) {
  using namespace type;

  unsigned const w = 8;
  Uninterpreted_Function f = declare_function(Boolean())(BitVector(w));
  bitvector x = new_bitvector(w);

  assertion(ctx, equal(f(x), f(x)) );
  BOOST_REQUIRE( solve(ctx) );

  assertion(ctx, nequal(f(x), f(x)) );
  BOOST_REQUIRE( !solve(ctx) );
}

BOOST_AUTO_TEST_CASE( equal_bitvector ) {
  using namespace type;
  unsigned const result_width = 4;
  unsigned const param_width = 8;
  Uninterpreted_Function f = declare_function(BitVector(result_width))(BitVector(param_width));
  bitvector x = new_bitvector(param_width);

  assertion(ctx, equal(f(x), f(x)) );
  BOOST_REQUIRE( solve(ctx) );

  assertion(ctx, nequal(f(x), f(x)) );
  BOOST_REQUIRE( !solve(ctx) );
}

BOOST_AUTO_TEST_CASE( functional_consistency ) {
  using namespace type;

  unsigned const w = 8;
  Uninterpreted_Function f = declare_function(Boolean())(BitVector(w));
  bitvector x = new_bitvector(w);
  bitvector y = new_bitvector(w);  

  // functional consistency:
  //   ( x == y ) --> ( f(x) == f(y) )
  assertion(ctx, equal(x, y) );

  assertion(ctx, equal(f(x), f(y)) );
  BOOST_REQUIRE( solve(ctx) );
}

BOOST_AUTO_TEST_CASE( two_arguments ) {
  using namespace type;
  unsigned const w = 8;

  Uninterpreted_Function f = declare_function(Boolean())(BitVector(w))(BitVector(w));
  bitvector x = new_bitvector(w);

  assertion(ctx, equal(f(x,x), f(x,x)));
  BOOST_REQUIRE( solve(ctx) );
}

BOOST_AUTO_TEST_CASE( three_arguments ) {
  using namespace type;
  unsigned const w = 8;

  Uninterpreted_Function f = declare_function(Boolean())(BitVector(w))(BitVector(w))(BitVector(w));
  bitvector x = new_bitvector(w);

  assertion(ctx, equal(f(x,x,x), f(x,x,x)));
  BOOST_REQUIRE( solve(ctx) );
}

BOOST_AUTO_TEST_CASE( variable_equality ) {
  using namespace type;

  unsigned const w = 8;
  Uninterpreted_Function f = declare_function(Boolean())(BitVector(w));
  Uninterpreted_Function g = declare_function(Boolean())(BitVector(w));

  bool cmp = (f == f);
  BOOST_CHECK( cmp );

  cmp = (g == f);
  BOOST_CHECK( !cmp );

  cmp = (f == g);
  BOOST_CHECK( !cmp );
}

BOOST_AUTO_TEST_SUITE_END() // QF_UF

//  vim: ft=cpp:ts=2:sw=2:expandtab
