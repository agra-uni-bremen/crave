#define BOOST_TEST_MODULE Distribution_t
#include <boost/test/unit_test.hpp>

#include <crave/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

//using namespace std;
using namespace crave;

using boost::format;

struct Context_Fixture {
  Context_Fixture ()
  : ctx()
  {
  }
  protected:
    Context ctx;
};

BOOST_FIXTURE_TEST_SUITE(Distribution_t, Context_Fixture )

BOOST_AUTO_TEST_CASE ( randv_dist_t1 )
{
  randv<int> v(NULL);
  v.addRange(0, 10);
  v.addRange(50, 75);
  v.addRange(100, 200);

  std::map<int, int> s;
  int total = 500000;
  for (uint i = 0; i < total; i++) {
    BOOST_REQUIRE(v.next());
    BOOST_REQUIRE((0 <= v && v <= 10) || (50 <= v && v <= 75) || (100 <= v && v <= 200));
    ++s[v];
  }
  int min = s[0], max = s[0];
  for (int i = 1; i <= 200; i++)
    if (s.find(i) != s.end()) { 
      if (s[i] < min) min = s[i];
      if (s[i] > max) max = s[i];
    }
  int avg = total / (11 + 26 + 101);
  BOOST_REQUIRE_LT(100. * (avg - min) / avg, 5);
  BOOST_REQUIRE_LT(100. * (max - avg) / avg, 5);
}

BOOST_AUTO_TEST_CASE ( randv_dist_t2 )
{
  randv<int> v(NULL);
  v.addRange(0, 10);
  v.addRange(50, 75);
  BOOST_CHECK_THROW ( v.addRange(30, 51), std::runtime_error );
}

BOOST_AUTO_TEST_CASE ( randv_dist_t3 )
{
  randv<char> v(NULL);
  v.addWeightedRange(1, 5, 50);
  v.addWeightedRange(10, 20, 20);
  v.addWeightedRange(-50, -50, 30);
  int cnt1 = 0, cnt2 = 0, cnt3 = 0;
  int total = 500000;
  for (uint i = 0; i < total; i++) {
    BOOST_REQUIRE(v.next());
    BOOST_REQUIRE((1 <= v && v <= 5) || (10 <= v && v <= 20) || (v == -50));
    if (1 <= v && v <= 5) cnt1++;
    if (10 <= v && v <= 20) cnt2++;
    if (v == -50) cnt3++;
  }
  double q = 50.0 / cnt1;
  BOOST_REQUIRE_LT(abs(cnt2 * q - 20), 0.2);
  BOOST_REQUIRE_LT(abs(cnt3 * q - 30), 0.2);
}

BOOST_AUTO_TEST_CASE ( randv_dist_t4 )
{
  randv<int> v(NULL);
  v.addRange(0, 10);
  v.addRange(50, 75);
  v.addRange(100, 200);
  v.resetDistribution();
  v.addRange(5000, 6000);
  int total = 100000;
  for (uint i = 0; i < total; i++) {
    BOOST_REQUIRE(v.next());
    BOOST_REQUIRE(5000 <= v && v <= 6000);
  }
}

/*
struct Dist {
  template <typename T1, typename T2>
  Dist & operator() (T1, T2) {return *this;}
};

template<typename T1, typename T2>
Dist dist( T1, T2 ) {
  return Dist();
}

template<typename T1, typename T2>
int interval(T1, T2) {
  return 1;
}

BOOST_AUTO_TEST_CASE ( static_dist_t )
{
  Generator<> gen;
  Variable<int> r;
  dist(gen,r)(1, 10000)( interval(2,10000), 1);

  int c=0;

  for (unsigned i = 0; i < 10; ++i) {
    gen();
    int rv = gen[r];
    BOOST_REQUIRE_MESSAGE( rv >=1 && rv <= 10000, "rv not a valid value: " << rv);
    if (rv ==1) {
      ++c;
    } else {
      --c;
    }
  }
  BOOST_CHECK_GT(c, 0);
}
*/

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
