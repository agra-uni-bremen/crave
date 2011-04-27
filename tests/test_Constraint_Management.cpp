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
//    constraint("x", a() != 2);
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

  while (it.next()) {
    std::cout << it << std::endl;
    assert(it.a + it.b == 4);
    assert(it.a * it.b == 4);
    it.constraint(it.a() != it.a || it.b() != it.b);
  }

//  it.disable_constraint_group("sum");
//  std::cout << it.next() << " " << it << std::endl;

//  it.disable_constraint_group("product");
//  std::cout << it.next() << " " << it << std::endl;

//  it.enable_constraint_group("sum");
//  std::cout << it.next() << " " << it << std::endl;

//  it.enable_constraint_group("product");
//  std::cout << it.next() << " " << it << std::endl;

//  it.disable_constraint_group("x");
//  std::cout << it.next() << " " << it << std::endl;

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

#define IMPL(a, b) !(a) || (b)

struct Item2 : public rand_obj {
  Item2() : x(this) {
    constraint(60 <= u().size() && u().size() <= 70);
    constraint(15 <= v().size() && v().size() <= 20);
    constraint(10 <= x() && x() <= 15);

    constraint.foreach(u, _i, u()[_i] == u()[_i - 1] + u()[_i - 2]);
    constraint.foreach(u, _i, IMPL(_i <= 1, u()[_i] == _i ) );

    constraint.foreach(v, _i, IMPL(_i < 4, v()[_i] == _i * 2));
    constraint.foreach(v, _i, v()[_i] < 10);

    constraint.foreach(w, _i, w()[_i] == w()[_i - 1] + ref(x));
    constraint.foreach(w, _i, IMPL(_i == 0, w()[_i] <= 5));
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
