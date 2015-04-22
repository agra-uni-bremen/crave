#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>

#include <crave/coverage/Coverage.hpp>

#include <vector>

//using namespace std;
using namespace crave;

BOOST_FIXTURE_TEST_SUITE(CoverageSampling, Context_Fixture )

BOOST_AUTO_TEST_CASE( basic_test )
{
  randv<unsigned> a, b, c, d, e;
  
  coverpoint cp1("coverpoint1");
  cp1.bins() = boost::assign::list_of( coverbin(a() == 0, 10) )( coverbin(a() == 1, 10) )( coverbin(a() == 2, 5) );

  coverpoint cp2("coverpoint2");
  cp2.bins() = boost::assign::list_of( coverbin(0 <= b() && b() < 10, 15) )( coverbin(10 <= b() && b() < 20, 10) );

  covergroup cg("covergroup1");
  cg.points() = boost::assign::list_of( cp1 )( cp2 );
  
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 5; j++) {
      a = i % 3;
      b = j * 11 % 20;
      cg.eval().assign(a(), a);
      cg.eval().assign(b(), b);
      cg.sample();
  }

  BOOST_CHECK(cg.covered());
}

BOOST_AUTO_TEST_SUITE_END() // CoverageSampling

//  vim: ft=cpp:ts=2:sw=2:expandtab
