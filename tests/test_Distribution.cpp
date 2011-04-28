#define BOOST_TEST_MODULE Distribution_t
#include <boost/test/unit_test.hpp>

#include <platzhalter/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

//using namespace std;
using namespace platzhalter;

using boost::format;

struct Context_Fixture {
  Context_Fixture ()
  : ctx(get_context())
  {
  }
  protected:
    Context ctx;
};

BOOST_FIXTURE_TEST_SUITE(Distribution_t, Context_Fixture )

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


BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
