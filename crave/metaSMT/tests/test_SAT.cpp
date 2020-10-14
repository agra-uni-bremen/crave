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

#define BOOST_TEST_MODULE test_sat
#include <boost/test/unit_test.hpp>

//internal includes
#include <metaSMT/frontend/SAT.hpp>
#include <metaSMT/transform/satClause.hpp>
#include <metaSMT/backend/MiniSAT.hpp>

//external includes
#include <boost/array.hpp>
#include <boost/proto/debug.hpp>
#include <boost/proto/fusion.hpp>
#include <boost/fusion/include/io.hpp>

using namespace metaSMT;

class sat_Fixture {
  protected:
};

using SAT::variable;
using SAT::new_variable;
using boost::proto::display_expr;

//template< typename Context, typename Expr >
//typename boost::result_of< transform::satClause( Expr )>::type
//evaluate ( Context const & , Expr const & ) {
//  return boost::result_of< transform::satClause( Expr )>::type::value;
//}

BOOST_FIXTURE_TEST_SUITE(sat_t, sat_Fixture )

BOOST_AUTO_TEST_CASE( syntax )
{
  SAT::variable x=new_variable(), y=new_variable(), z=new_variable();

  display_expr( 
    x + !y + !~z
  );

  std::cout << transform::satClauseArity() (
      x+!y+!~z
 ) << std::endl;


  display_expr(
      x+ y + (-z)
  );

  std::cout << transform::satClause() (
      x + y + -(-(-z))
 ) << std::endl;

}

BOOST_AUTO_TEST_SUITE_END() //QF_BV

//  vim: ft=cpp:ts=2:sw=2:expandtab
