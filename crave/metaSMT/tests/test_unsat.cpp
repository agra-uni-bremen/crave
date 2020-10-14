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

#ifdef _MSC_VER

#if _MSC_VER < 1900
#define DISABLE_UNSAT_TESTS
#endif

#elif __cplusplus <= 199711L

#define DISABLE_UNSAT_TESTS

#endif


#ifdef DISABLE_UNSAT_TESTS

#pragma message("Disable test_unsat testcases which require C++11")
#undef DISABLE_UNSAT_TESTS

#else

#include <boost/test/unit_test.hpp>
#include <string>

#include <metaSMT/support/contradiction_analysis.hpp>
#include <metaSMT/frontend/Logic.hpp>

#include <boost/fusion/adapted/boost_tuple.hpp>
#include <algorithm>

using namespace metaSMT;
using namespace metaSMT::solver;
using namespace metaSMT::logic;
using std::cout;
using std::endl;

std::ostream & operator<< (std::ostream & out, std::vector<unsigned> const & v)
{
 out <<"( ";
 for(unsigned u : v)  {
   out << u << ' ';
 }
 out <<')';
 return out;
}

std::ostream & operator<< (std::ostream & out, std::vector<std::vector<unsigned> > const & v)
{
 out <<"( ";
 for(std::vector<unsigned> const & u : v)  {
   out << u << ' ';
 }
 out <<')';
 return out;
}

//vergleich von vector<unsigned>
bool cmp_vec(std::vector<unsigned> const &a, std::vector<unsigned> const &b)
{
	if(a.size() == b.size())
	{
		for (unsigned i = 0; i < a.size(); i++) {
			if(a[i] != b[i])
			{
			  return a[i] < b[i];
			}
		}
	} else {
		return a.size() < b.size();
	}
  return false;
}

//sortieren
void sort_results( std::vector<std::vector<unsigned> > &results )
{
	for(unsigned i = 0; i < results.size();i++)
	{
	  sort(results[i].begin(), results[i].end());
	}
	sort(results.begin(), results.end(), cmp_vec);
}


BOOST_FIXTURE_TEST_SUITE(unsat, Solver_Fixture )

BOOST_AUTO_TEST_CASE( one_true )
{
  BOOST_REQUIRE( solve(ctx) );

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, boost::make_tuple( True ) );

  BOOST_REQUIRE_EQUAL(result.size(), 0);
}

BOOST_AUTO_TEST_CASE( one_true_vec )
{
  BOOST_REQUIRE( solve(ctx) );
  typedef ContextType::result_type result_type;
  
  result_type x = evaluate(ctx,True);

  std::vector<result_type> vec;
  vec.push_back(x);
  
  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, vec );

  BOOST_REQUIRE_EQUAL(result.size(), 0);
}

BOOST_AUTO_TEST_CASE( one_false )
{
  BOOST_REQUIRE( solve(ctx) );

  std::vector< std::vector<unsigned> > result =
     contradiction_analysis(ctx, boost::make_tuple( False ));

  BOOST_REQUIRE_EQUAL(result.size(), 1);

}

BOOST_AUTO_TEST_CASE( one_false_vec )
{
  BOOST_REQUIRE( solve(ctx) );
  typedef ContextType::result_type result_type;
  
  result_type x = evaluate(ctx,False);

  std::vector<result_type> vec;
  vec.push_back(x);
  
  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, vec );

  BOOST_REQUIRE_EQUAL(result.size(), 1);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[0][0], 0);
}

BOOST_AUTO_TEST_CASE( two_false )
{
  BOOST_REQUIRE( solve(ctx) );

  std::vector< std::vector<unsigned> > result =
      contradiction_analysis(ctx, boost::make_tuple( False, False ));

  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[1].size(), 1);
  BOOST_REQUIRE(result[0][0]==0 || result[1][0]==0);
  BOOST_REQUIRE(result[0][0]==1 || result[1][0]==1);

}

BOOST_AUTO_TEST_CASE( two_false_vec )
{
  BOOST_REQUIRE( solve(ctx) );
  typedef ContextType::result_type result_type;
  
  result_type x = evaluate(ctx,False);
  result_type y = evaluate(ctx,False);
  
  std::vector<result_type> vec;
  vec.push_back(x);
  vec.push_back(y);

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, vec );

  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[1].size(), 1);
  BOOST_REQUIRE(result[0][0]==0 || result[1][0]==0);
  BOOST_REQUIRE(result[0][0]==1 || result[1][0]==1);

}

BOOST_AUTO_TEST_CASE( three_false )
{
  BOOST_REQUIRE( solve(ctx) );

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, boost::make_tuple(False, False, False) );

  BOOST_REQUIRE_EQUAL(result.size(), 3);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[1].size(), 1);
  BOOST_REQUIRE_EQUAL(result[2].size(), 1);
  BOOST_REQUIRE(result[0][0] == 0 || result[1][0] == 0 || result[2][0] == 0);
  BOOST_REQUIRE(result[0][0] == 1 || result[1][0] == 1 || result[2][0] == 1);
  BOOST_REQUIRE(result[0][0] == 2 || result[1][0] == 2 || result[2][0] == 2);
   

}

BOOST_AUTO_TEST_CASE( three_false_vec )
{
  BOOST_REQUIRE( solve(ctx) );
  typedef ContextType::result_type result_type;
  
  result_type x = evaluate(ctx,False);
  result_type y = evaluate(ctx,False);
  result_type z = evaluate(ctx,False);
  std::vector<result_type> vec;
  vec.push_back(x);
  vec.push_back(y);
  vec.push_back(z);

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, vec );

  BOOST_REQUIRE_EQUAL(result.size(), 3);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[1].size(), 1);
  BOOST_REQUIRE_EQUAL(result[2].size(), 1);
  BOOST_REQUIRE(result[0][0] == 0 || result[1][0] == 0 || result[2][0] == 0);
  BOOST_REQUIRE(result[0][0] == 1 || result[1][0] == 1 || result[2][0] == 1);
  BOOST_REQUIRE(result[0][0] == 2 || result[1][0] == 2 || result[2][0] == 2);
}

BOOST_AUTO_TEST_CASE( two_conflicts_1 )
{
  BOOST_REQUIRE( solve(ctx) );

  predicate b = new_variable();
  predicate d = new_variable();
  
  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, boost::make_tuple(True, False, nequal(d,b), equal(d, b) ));
  sort_results(result);

  cout << result << endl;
  //cout << result.size() << " result.size() " << endl;

  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[1].size(), 2);
  BOOST_REQUIRE_EQUAL(result[0][0], 1);

  std::vector<unsigned> expected = { 2, 3 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( two_conflicts_1_vec )
{
  BOOST_REQUIRE( solve(ctx) );
  typedef ContextType::result_type result_type;
  
  predicate b = new_variable();
  predicate c = new_variable();

  result_type x = evaluate(ctx,True);
  result_type y = evaluate(ctx,False);
  result_type z1 = evaluate(ctx,nequal(b,c));
  result_type z2 = evaluate(ctx,equal(b,c));

  std::vector<result_type> vec;
  vec.push_back(x);
  vec.push_back(y);
  vec.push_back(z1);
  vec.push_back(z2);

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, vec );
  
  sort_results(result);

  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[1].size(), 2);
  BOOST_REQUIRE_EQUAL(result[0][0], 1);

  std::vector<unsigned> expected = { 2, 3 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( two_conflicts_2 )
{
  BOOST_REQUIRE( solve(ctx) );

  predicate b = new_variable();
  predicate c = new_variable();
  predicate d = new_variable();
  
  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, boost::make_tuple(equal(c,b), nequal(d,b), nequal(c,b), equal(d, b) ));
  sort_results(result);

  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 2);
  BOOST_REQUIRE_EQUAL(result[1].size(), 2);
  std::vector<unsigned> expected;

  expected = { 0, 2 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());

  expected = { 1, 3 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( two_conflicts_2_vec )
{
  BOOST_REQUIRE( solve(ctx) );
  typedef ContextType::result_type result_type;
  
  predicate a = new_variable();
  predicate b = new_variable();
  predicate c = new_variable();
 
  result_type x1 = evaluate(ctx,equal(a,b));
  result_type x2 = evaluate(ctx,nequal(c,b));
  result_type z1 = evaluate(ctx,nequal(a,b));
  result_type z2 = evaluate(ctx,equal(b,c));

  std::vector<result_type> vec;
  vec.push_back(x1);
  vec.push_back(x2);
  vec.push_back(z1);
  vec.push_back(z2);

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, vec );
  
  sort_results(result);

  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 2);
  BOOST_REQUIRE_EQUAL(result[1].size(), 2);
  std::vector<unsigned> expected;

  expected= { 0, 2 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());

  expected= { 1, 3 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( two_conflicts_3 )
{
  BOOST_REQUIRE( solve(ctx) );

  predicate b = new_variable();
  predicate d = new_variable();
  
  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, boost::make_tuple(True, nequal(d,b), False, equal(d, b) ));
  sort_results(result);

  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[1].size(), 2);
  BOOST_REQUIRE_EQUAL(result[0][0], 2);

  std::vector<unsigned> expected = { 1, 3 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( two_conflicts_3_vec )
{
  BOOST_REQUIRE( solve(ctx) );
  typedef ContextType::result_type result_type;
  
  predicate a = new_variable();
  predicate b = new_variable();
 
  result_type x1 = evaluate(ctx,True);
  result_type x2 = evaluate(ctx,nequal(a,b));
  result_type z1 = evaluate(ctx,False);
  result_type z2 = evaluate(ctx,equal(a,b));

  std::vector<result_type> vec;
  vec.push_back(x1);
  vec.push_back(x2);
  vec.push_back(z1);
  vec.push_back(z2);

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, vec );
  
  sort_results(result);

  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[1].size(), 2);
  BOOST_REQUIRE_EQUAL(result[0][0], 2);

  std::vector<unsigned> expected = { 1, 3 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( double_conflict_1 )
{
  BOOST_REQUIRE( solve(ctx) );
  
  predicate b = new_variable();
  predicate d = new_variable();
  
  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, boost::make_tuple(True, nequal(d,b), equal(d, b), nequal(d,b) ));
  sort_results(result);

  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 2);
  BOOST_REQUIRE_EQUAL(result[1].size(), 2);

std::vector<unsigned> expected;

  expected = { 1, 2 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());

  expected = { 2, 3 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( double_conflict_1_vec )
{
  BOOST_REQUIRE( solve(ctx) );
  typedef ContextType::result_type result_type;
  
  predicate a = new_variable();
  predicate b = new_variable();
 
  result_type x1 = evaluate(ctx,True);
  result_type x2 = evaluate(ctx,nequal(a,b));
  result_type z1 = evaluate(ctx,equal(a,b));
  result_type z2 = evaluate(ctx,nequal(a,b));

  std::vector<result_type> vec;
  vec.push_back(x1);
  vec.push_back(x2);
  vec.push_back(z1);
  vec.push_back(z2);

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, vec );
  
  sort_results(result);

  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 2);
  BOOST_REQUIRE_EQUAL(result[1].size(), 2);

std::vector<unsigned> expected;

  expected = { 1, 2 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());

  expected = { 2, 3 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( double_conflicts_1)
{
  BOOST_REQUIRE( solve(ctx) );

  predicate a = new_variable();
  predicate b = new_variable();
  predicate c = new_variable();
  predicate d = new_variable(); 

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, boost::make_tuple( 
      True,
      equal(d,b),
      False,
      nequal(d,b),
      False,
      equal(a,c),
      nequal(a,c),
      True
    ));

  sort_results(result);

  BOOST_REQUIRE_EQUAL(result.size(), 4);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[1].size(), 1);
  BOOST_REQUIRE_EQUAL(result[2].size(), 2);
  BOOST_REQUIRE_EQUAL(result[3].size(), 2);

  std::vector<unsigned> expected;

  expected = { 2 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());

  expected = { 4 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());

  expected = { 1, 3 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[2].begin(), result[2].end(), expected.begin(), expected.end());

  expected = { 5, 6 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[3].begin(), result[3].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( double_conflicts_1_vec)
{
  BOOST_REQUIRE( solve(ctx) );
  typedef ContextType::result_type result_type;
  
  predicate a = new_variable();
  predicate b = new_variable();
  predicate c = new_variable();
  predicate d = new_variable();

  std::vector<result_type> vec;

  vec.push_back( evaluate(ctx,True) );
  vec.push_back( evaluate(ctx,equal(a,b)) );
  vec.push_back( evaluate(ctx,False) );
  vec.push_back( evaluate(ctx,nequal(a,b)) );
  vec.push_back( evaluate(ctx,False) );
  vec.push_back( evaluate(ctx,equal(c,d)) );
  vec.push_back( evaluate(ctx,nequal(c,d)) );
  vec.push_back( evaluate(ctx,True) );

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, vec );
  
  sort_results(result);

  BOOST_REQUIRE_EQUAL(result.size(), 4);
  BOOST_REQUIRE_EQUAL(result[0].size(), 1);
  BOOST_REQUIRE_EQUAL(result[1].size(), 1);
  BOOST_REQUIRE_EQUAL(result[2].size(), 2);
  BOOST_REQUIRE_EQUAL(result[3].size(), 2);

  std::vector<unsigned> expected;

  expected = { 2 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());
 
  expected = { 4 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());

  expected = { 1, 3 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[2].begin(), result[2].end(), expected.begin(), expected.end());
 
  expected = { 5, 6 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[3].begin(), result[3].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( unsolve_conflict)
{
  BOOST_REQUIRE( solve(ctx) );

  predicate a = new_variable();
  predicate b = new_variable();
  predicate c = new_variable();
  
  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, boost::make_tuple( nequal(a,b), nequal(b,c), nequal(a,c)) );


  BOOST_REQUIRE_EQUAL(result.size(), 1);
  std::vector<unsigned> expected;

  expected = { 0, 1, 2 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE( unsolve_conflict_vec)
{
  BOOST_REQUIRE( solve(ctx) );
  typedef ContextType::result_type result_type;
  
  predicate a = new_variable();
  predicate b = new_variable();
  predicate c = new_variable();

  result_type x1 = evaluate(ctx,nequal(a,b));
  result_type x2 = evaluate(ctx,nequal(b,c));
  result_type x3 = evaluate(ctx,nequal(a,c));

  std::vector<result_type> vec;
  vec.push_back(x1);
  vec.push_back(x2);
  vec.push_back(x3);

  std::vector< std::vector<unsigned> > result =
    contradiction_analysis(ctx, vec );
  
  sort_results(result);

  BOOST_REQUIRE_EQUAL(result.size(), 1);
  std::vector<unsigned> expected;

  expected = { 0, 1, 2 };
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_SUITE_END() //Solver

#endif
//  vim: ft=cpp:ts=2:sw=2:expandtab
