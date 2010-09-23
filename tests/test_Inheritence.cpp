#define BOOST_TEST_MODULE Inheritence_t
#include <boost/test/unit_test.hpp>

#include <platzhalter/Constraint.hpp>
#include <platzhalter/Context.hpp>
#include <platzhalter/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

//using namespace std;
using boost::format;
using namespace platzhalter;

class Constraint_base : public Generator<> {
  public:
    Constraint_base() 
      : Generator<>()
      , constraint(*this)
      , soft_constraint( constraint )
    {}

  protected:
    Generator<> & constraint;
    Soft_Generator<> soft_constraint;
};

class Constraint1 : public Constraint_base {
  public:
    Variable<unsigned> x;
    
    Constraint1 () {
      constraint( x<10 );
    }
};

class Constraint2 : public Constraint1 {
  public:
    Constraint2 () {
      constraint( x>6 );
    }
};

class item : public rand_obj {
public:
  item(rand_obj* parent) : rand_obj(parent), a(this), b(this), c(this) {
    constraint(a() + b() == c());
  }
public:
  randv<int> a;
  randv<int> b;
  randv<int> c;
};

class item1 : public item {
public:
  item1(rand_obj* parent) : item(parent) {
    constraint(10 <= a() && a() <= 20);
    constraint(a() +  b() + c() <= 200);
  }
};

class item2 : public item1 {
public:
  item2(rand_obj* parent) : item1(parent), d(this) {
    constraint(a() + b() + c() == 100);
  }
  randv<int> d;
};

class obj : public rand_obj {
public:
  obj(rand_obj* parent) : rand_obj(parent), a(this), b(this), c(this), d(this), e(this), f(this) { 
    e.range('a', 'z');
    f.range('A', 'Z');
  }
  randv<int> a;
  randv<unsigned int> b;
  randv<short> c;
  randv<unsigned short> d;
  randv<char> e;
  randv<unsigned char> f;

  friend ostream& operator<<(ostream& os, const obj& o) { 
    os << "(" << o.a << " " << o.b << " " << o.c << " "  << o.d << " "  << o.e << " "  << o.f << ")"; 
    return os; 
  }
};

class obj1 : public obj {
public:
  obj1(rand_obj* parent) : obj(parent) { 
    e.range('A', 'Z');
    f.range('a', 'z');
  }
};

class obj2 : public rand_obj {
public:
  obj2(rand_obj* parent) : rand_obj(parent), g(this), h(this), i(this), j(this), k(this), l(this) { }
  randv<long> g;
  randv<unsigned long> h;
  randv<long long> i;
  randv<unsigned long long> j;
  randv<bool> k;
  obj1 l;

  friend ostream& operator<<(ostream& os, const obj2& o1) { 
    os << o1.l << " " << o1.g << " " << o1.h << " "  << o1.i << " "  << o1.j << " "  << o1.k; 
    return os; 
  }
};


struct Context_Fixture {};

BOOST_FIXTURE_TEST_SUITE(Inheritence_t, Context_Fixture )

BOOST_AUTO_TEST_CASE( t1 )
{
  Constraint2 c2;

  c2();
  unsigned r = c2[c2.x];
  BOOST_REQUIRE_LT( r, 10);
  BOOST_REQUIRE_GT( r,  6);

  c2( c2.x != r )();
  r = c2[c2.x];
  BOOST_REQUIRE_LT( r, 10);
  BOOST_REQUIRE_GT( r,  6);

  c2( c2.x != r )();
  r = c2[c2.x];
  BOOST_REQUIRE_LT( r, 10);
  BOOST_REQUIRE_GT( r,  6);

  c2( c2.x != r );
  BOOST_REQUIRE( ! c2.next() );
}

BOOST_AUTO_TEST_CASE( t2 )
{
  item it(0);
  it.next();
  std::cout << it.a << " " << it.b << " " << it.c << std::endl;
  BOOST_REQUIRE(it.a + it.b == it.c);
}

BOOST_AUTO_TEST_CASE( t3 )
{
  item1 it(0);
  it.next();
  std::cout << it.a << " " << it.b << " " << it.c << std::endl;
  BOOST_REQUIRE(it.a + it.b == it.c);
  BOOST_REQUIRE(10 <= it.a && it.a <= 20);
}

BOOST_AUTO_TEST_CASE( t4 )
{
  item2 it(0);
  it.next();
  std::cout << it.a << " " << it.b << " " << it.c << std::endl;
  BOOST_REQUIRE(it.a + it.b == it.c);
  BOOST_REQUIRE(10 <= it.a && it.a <= 20);
  BOOST_REQUIRE(it.a + it.b + it.c == 100);
}

BOOST_AUTO_TEST_CASE( t5 )
{
  obj* it = new obj(0);
  for (int i = 0; i < 20; i++) {
    it->next();
    std::cout << *it << std::endl;
  }

  obj2* it1 = new obj2(0);
  for (int i = 0; i < 20; i++) {
    it1->next();
    std::cout << *it1 << std::endl;
  }
}

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
