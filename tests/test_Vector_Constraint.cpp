#define BOOST_TEST_MODULE Vector_Constraint_t
#include <boost/test/unit_test.hpp>

#include <platzhalter/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

using boost::format;
using namespace platzhalter;

#define ref(x) reference(x)
#define IMPL(a, b) !(a) || (b)

struct Context_Fixture {};

BOOST_FIXTURE_TEST_SUITE(Vector_Constraint_t, Context_Fixture )

struct Item2 : public rand_obj {
  Item2() : x(this) {
    constraint(60 <= u().size() && u().size() <= 70);
    constraint(15 <= v().size() && v().size() <= 20);
    x.range(100, 200);

    constraint.foreach(u, _i, u()[_i] == u()[_i - 1] + u()[_i - 2]);
    constraint.foreach(u, _i, IMPL(_i <= 1, u()[_i] == _i ) );

    constraint.foreach(v, _i, IMPL(_i < 4, v()[_i] == _i * 2));
    constraint.foreach(v, _i, v()[_i] < 10);

    constraint.foreach(w, _i, w()[_i] == w()[_i - 1] + ref(x));
  }

  rand_vec<unsigned int> u;
  rand_vec<int> v;
  rand_vec<unsigned int> w;
  randv<unsigned int> x;
};

BOOST_AUTO_TEST_CASE ( vector_test )
{
  std::cout << "vector_test" << std::endl;
  Item2 it;
  it.next();

  it.u.print();
  it.v.print();
  it.w.print();
}

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
