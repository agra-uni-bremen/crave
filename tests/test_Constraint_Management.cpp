#define BOOST_TEST_MODULE Constraint_Management_t
#include <boost/test/unit_test.hpp>

#include <platzhalter/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

using boost::format;
using namespace platzhalter;

class Item : public rand_obj {
public:
  Item() : rand_obj(0), a(this), b(this) {
    constraint("sum", a() + b() == 4);
    constraint("product", a() * b() == 4);
    constraint(a() < 10);
    constraint(b() < 10);
    constraint("x", a() != 2);
  }
  randv<int> a;
  randv<int> b;

  friend ostream& operator<<(ostream& os, const Item& it) { 
    os << it.a << " " << it.b; 
    return os; 
  }
};

struct Context_Fixture {};

BOOST_FIXTURE_TEST_SUITE(Constraint_Management_t, Context_Fixture )

BOOST_AUTO_TEST_CASE( t1 )
{
  Item it;
  std::cout << it.next() << " " << it << std::endl;

  it.disable_constraint_group("sum");
  std::cout << it.next() << " " << it << std::endl;

  it.disable_constraint_group("product");
  std::cout << it.next() << " " << it << std::endl;

  it.enable_constraint_group("sum");
  std::cout << it.next() << " " << it << std::endl;

  it.enable_constraint_group("product");
  std::cout << it.next() << " " << it << std::endl;

  it.disable_constraint_group("x");
  std::cout << it.next() << " " << it << std::endl;
}

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
