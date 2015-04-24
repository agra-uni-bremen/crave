#include <boost/test/unit_test.hpp>
#include <boost/assign.hpp>

#include <crave/coverage/Coverage.hpp>

#include <vector>

// using namespace std;
using namespace crave;

BOOST_FIXTURE_TEST_SUITE(CoverageSampling, Context_Fixture)

BOOST_AUTO_TEST_CASE(basic_test) {
  randv<unsigned> a, b, c, d, e;

  coverpoint cp1("coverpoint1");
  {
    std::vector<coverbin> tmp = boost::assign::list_of(coverbin(a() == 0, 10))(
        coverbin(a() == 1, 10))(coverbin(a() == 2, 5));
    cp1.bins() = tmp;
  }

  coverpoint cp2("coverpoint2");

  {
    std::vector<coverbin> tmp = boost::assign::list_of(coverbin(a() == 0, 10))(
        coverbin(a() == 1, 10))(coverbin(a() == 2, 5));
    cp2.bins() = tmp;
  }

  covergroup cg("covergroup1");
  {
    std::vector<coverpoint> tmp = boost::assign::list_of(cp1)(cp2);
    cg.points() = tmp;
  }

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

BOOST_AUTO_TEST_SUITE_END()  // CoverageSampling

//  vim: ft=cpp:ts=2:sw=2:expandtab
