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
  BOOST_REQUIRE(30 <= it.v.size() && it.v.size() <= 50);
  BOOST_REQUIRE(it.v[0] == 0);
  BOOST_REQUIRE(it.v[1] == 1);
  for (uint i = 2; i < it.v.size(); i++)
    BOOST_REQUIRE(it.v[i] == it.v[i - 1] + it.v[i - 2]);
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
  BOOST_REQUIRE(it.u.size() == 0);
  BOOST_REQUIRE(it.v.size() > 0);
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
  BOOST_REQUIRE(5 <= it.v.size() && it.v.size() <= 10);
  for (uint i = 0; i < it.v.size(); i++)
    BOOST_REQUIRE(100 <= it.v[i] && it.v[i] <= 200);
}

struct Item3 : public rand_obj {
  Item3() : v(this) {
    constraint(v().size() == 1000);
    constraint.foreach(v, _i, v()[_i] < 1000);
    constraint.unique(v);
  }

  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( unique_test_1 )
{
  Item3 it;
  it.next();
  BOOST_REQUIRE(it.v.size() == 1000);
  for (uint i = 0; i < it.v.size(); i++) {
    BOOST_REQUIRE(it.v[i] < 1000);
    for (uint j = 0; j < i; j++) 
      BOOST_REQUIRE(it.v[i] != it.v[j]);
  }
}

BOOST_AUTO_TEST_CASE ( unique_test_2 )
{
  rand_vec<unsigned int> v(NULL);
  Generator<> gen;
  gen(v().size() == 11);
  gen.foreach(v, _i, v()[_i] < 10);  

  gen.unique(v);
  BOOST_REQUIRE(!gen.next());

  gen.non_unique(v);
  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(v.size() == 11);
  for (uint i = 0; i < v.size(); i++) {
    std::cout << v[i] << " ";
    BOOST_REQUIRE(v[i] < 10);
  }
  std::cout << std::endl;
}

struct Item4 : public rand_obj {
  Item4() : v(this) {
    constraint(v().size() == 10);
    constraint.foreach("c1", v, _i, v()[_i] <= 100);
    constraint.foreach("c2", v, _i, v()[_i] > 100);
  }

  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( constraint_management_test )
{
  Item4 it;
  BOOST_REQUIRE( !it.next() );

  it.disable_constraint("c1");
  BOOST_REQUIRE( it.next() );
  for (uint i = 0; i < it.v.size(); i++)
    BOOST_REQUIRE( it.v[i] > 100 );

  it.enable_constraint("c1");
  it.disable_constraint("c2");
  BOOST_REQUIRE( it.next() );
  for (uint i = 0; i < it.v.size(); i++)
    BOOST_REQUIRE( it.v[i] <= 100 );

  it.enable_constraint("c2");
  BOOST_REQUIRE( !it.next() );
}

struct Item5 : public rand_obj {
  Item5() : v(this) {
    constraint(v().size() == 50);
    constraint.foreach(v, _i, IMPL(_i < 25, v()[_i] == _i));
    constraint.foreach(v, _i, IMPL(_i == 25, v()[_i] == 0));
    constraint.foreach(v, _i, IMPL(_i > 25, v()[_i] + _i == 200));
  }

  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( index_constraint_test )
{
  Item5 it;
  it.next();
  BOOST_REQUIRE(it.v.size() == 50);
  for (uint i = 0; i < it.v.size(); i++) {
    if (i < 25) BOOST_REQUIRE(it.v[i] == i);
    else if (i > 25) BOOST_REQUIRE(it.v[i] + i == 200);
    else BOOST_REQUIRE(it.v[i] == 0);
  }
}

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
