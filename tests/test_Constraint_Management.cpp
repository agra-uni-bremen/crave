#define BOOST_TEST_MODULE Constraint_Management_t
#include <boost/test/unit_test.hpp>

#include <platzhalter/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

using boost::format;
using namespace platzhalter;

#define ref(x) reference(x)

struct Context_Fixture {};

BOOST_FIXTURE_TEST_SUITE(Constraint_Management_t, Context_Fixture )

class Item : public rand_obj {
public:
  Item() : rand_obj(), a(this), b(this) {
    constraint("sum", a() + b() == 4);
    constraint("product", a() * b() == 4);
    constraint(a() < 10u);
    constraint(b() < 10u);
    constraint("x", a() != 2u);
  }
  randv<uint> a;
  randv<uint> b;

  friend ostream& operator<<(ostream& os, const Item& it) { 
    os << it.a << " " << it.b; 
    return os; 
  }
};

BOOST_AUTO_TEST_CASE( t1 )
{
  Item it;

//  BOOST_REQUIRE(!it.next());
  it.next();
  std::cout << it << std::endl;


  it.disable_constraint("sum");
  BOOST_REQUIRE(it.next());
  std::cout << it << std::endl;

  it.disable_constraint("product");
  BOOST_REQUIRE(it.next());
  std::cout << it << std::endl;

  it.enable_constraint("sum");
  BOOST_REQUIRE(it.next());
  std::cout << it << std::endl;

  it.enable_constraint("product");
  BOOST_REQUIRE(!it.next());

  it.disable_constraint("x");
  BOOST_REQUIRE(it.next());
  std::cout << it << std::endl;
  BOOST_REQUIRE(it.a == 2 && it.b == 2);
}

class Item1 : public rand_obj {
public:
  Item1() : rand_obj(), a(this) {
    constraint("abc", a() == 4);
    constraint("def", a() == 3);
    constraint("abc", a() == 3);
  }
  randv<uint> a;
};

BOOST_AUTO_TEST_CASE( t2 )
{
  BOOST_CHECK_THROW ( Item1 it, std::runtime_error );
}


BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
