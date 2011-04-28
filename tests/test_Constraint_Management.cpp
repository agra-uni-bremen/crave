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
    constraint(a() < 10);
    constraint(b() < 10);
    constraint("x", a() != 2);
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

  BOOST_REQUIRE(!it.next());

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

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
