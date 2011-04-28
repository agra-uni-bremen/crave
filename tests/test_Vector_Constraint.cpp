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

struct Item : public rand_obj {
  Item() : v(this) {
    constraint(30 <= v().size() && v().size() <= 50);
    constraint.foreach(v, _i, v()[_i] == v()[_i - 1] + v()[_i - 2]);
    constraint.foreach(v, _i, IMPL(_i <= 1, v()[_i] == _i ) );
  }

  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( fibo_test )
{
  Item it;
  it.next();
  BOOST_CHECK(30 <= it.v.size() && it.v.size() <= 50);
  BOOST_CHECK(it.v[0] == 0);
  BOOST_CHECK(it.v[1] == 1);
  for (uint i = 2; i < it.v.size(); i++)
    BOOST_CHECK(it.v[i] == it.v[i - 1] + it.v[i - 2]);
}

struct Item1 : public rand_obj {
  Item1() : v(this) {
  }

  __rand_vec<unsigned int> u;
  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( free_vector_test )
{
  Item1 it;
  it.next();
  BOOST_CHECK(it.u.size() == 0);
  BOOST_CHECK(it.v.size() > 0);
  for (uint i = 0; i < it.v.size(); i++)
    std::cout << it.v[i] << " ";
  std::cout << std::endl;
}

struct Item2 : public rand_obj {
  Item2() : v(this) {
    constraint.foreach(v, _i, 100 <= v()[_i] && v()[_i] <= 200);
  }

  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( default_size_test )
{
  Item2 it;
  it.next();
  BOOST_CHECK(5 <= it.v.size() && it.v.size() <= 10);
  for (uint i = 0; i < it.v.size(); i++)
    BOOST_CHECK(100 <= it.v[i] && it.v[i] <= 200);
}

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
