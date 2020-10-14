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
#include <metaSMT/frontend/Logic.hpp>
#include <metaSMT/API/Assumption.hpp>
#include <metaSMT/API/Evaluator.hpp>

using namespace metaSMT;
using namespace metaSMT::solver;
using namespace metaSMT::logic;

namespace metaSMT {
  template <>
  struct Evaluator<char> : public boost::mpl::true_ {
    template < typename Context >
    static typename Context::result_type eval(Context &ctx, char const &c) {
      // assert( (c == '1' || c == '0' || c == 'X' || c == 'x') &&
      //         "has to be 0, 1, X, or x character" );

      if ( c == '1' ) {
        return evaluate(ctx, True);
      }
      else if ( c == '0' ) {
        return evaluate(ctx, False);
      }
      else if ( c == 'X' || c == 'x' ) {
        // prefer false in case of a don't care
        return evaluate(ctx, False);
      }

      // error
      std::string message = "trying evaluate an unsupported character: ";
      message += c;
      throw std::runtime_error(message);
    }
  }; // Evaluator
} // metaSMT

BOOST_FIXTURE_TEST_SUITE(eval, Solver_Fixture )

BOOST_AUTO_TEST_CASE( char_evaluator )
{
  predicate x = new_variable();
  assertion(ctx, logic::equal(x, x));
  BOOST_REQUIRE( solve(ctx) );

  bool xd;
  assumption(ctx, logic::equal(x, '1'));
  BOOST_REQUIRE( solve(ctx) );
  xd = read_value(ctx, x);
  BOOST_CHECK_EQUAL(xd, true);

  assumption(ctx, logic::equal(x, '0'));
  BOOST_REQUIRE( solve(ctx) );
  xd = read_value(ctx, x);
  BOOST_CHECK_EQUAL(xd, false);

  assumption(ctx, logic::equal(x, 'X'));
  BOOST_REQUIRE( solve(ctx) );
  xd = read_value(ctx, x);
  BOOST_CHECK_EQUAL(xd, false);

  BOOST_CHECK_THROW(
    assertion(ctx, logic::equal(x, 'U'))
  , std::runtime_error
  );
}

BOOST_AUTO_TEST_SUITE_END() // eval

//  vim: ft=cpp:ts=2:sw=2:expandtab
