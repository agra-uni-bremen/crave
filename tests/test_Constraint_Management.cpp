#define BOOST_TEST_MODULE Constraint_Management_t
#include <boost/test/unit_test.hpp>

#include <platzhalter/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

using boost::format;
using namespace platzhalter;

#define ref(x) reference(x)

class Item : public rand_obj {
public:
  Item() : rand_obj(), a(this), b(this) {
    constraint("sum", a() + b() == 4);
    constraint("product", a() * b() == 4);
    constraint(a() < 10);
    constraint(b() < 10);
//    constraint("x", a() != 2);
  }
  randv<uint> a;
  randv<uint> b;

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

  while (it.next()) {
    std::cout << it << std::endl;
    assert(it.a + it.b == 4);
    assert(it.a * it.b == 4);
    it.constraint(it.a() != it.a || it.b() != it.b);
  }
/*
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
*/
}

struct Item1 : public rand_obj {
  Item1() : x(this), pivot(0) {
    constraint("c1", x() * x() >= 24);
    constraint("c2", x() <= ref(pivot));
  }

  bool next() {
    int lower = 0;
    int upper = 100;
    while (lower < upper) {
      std::cout << lower << " " << upper << std::endl;
      pivot = (upper + lower) / 2;
      if (constraint.next())
        upper = x;
      else
        lower = pivot + 1;
    }
    return true;
  }

  randv<int> x;
  int pivot;
};

BOOST_AUTO_TEST_CASE ( binary_search_test )
{
  Item1 it;
  std::cout << it.next() << " " << it.x << std::endl;
}

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
