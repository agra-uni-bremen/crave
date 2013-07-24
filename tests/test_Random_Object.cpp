#include <boost/test/unit_test.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

//using namespace std;
using boost::format;
using namespace crave;

enum color_enum {
  RED,
  GREEN,
  BLUE
};
CRAVE_ENUM(color_enum, (RED)(GREEN)(BLUE));

enum football_enum {
  GK,  // Goalkeeper
  SW,  // Sweeper
  LWB, // Left-Wing-Back
  LB,  // Left-Back
  LCB, // Left-Centre-Back
  RCB, // Right-Centre-Back
  RB,  // Right-Back
  RWB, // Right-Wing-Back
  DM,  // Defensive Midfielder
  LM,  // Left Wide Midfielder
  CM,  // Centre Midfielder
  RM,  // Right Wide Midfielder
  AM,  // Attacking Midfielder
  LW,  // Left Winger (Striker)
  SS,  // Secondary Striker
  RW,  // Right Winger
  CF   // Centre Striker
};
CRAVE_ENUM(football_enum, (GK)(SW)(LWB)(LB)(LCB)(RCB)(RB)(RWB)(DM)(LM)(CM)(RM)(AM)(LW)(SS)(RW)(CF));

BOOST_FIXTURE_TEST_SUITE(Random_Object_t, Context_Fixture )

class my_rand_obj : public rand_obj {
public:
  randv<color_enum> color;
  randv<int> x;

  my_rand_obj(rand_obj* parent = 0) : rand_obj(parent), color(this), x(this) {
    constraint(color() == x());
  }
};

BOOST_AUTO_TEST_CASE( t_rand_enum )
{
  my_rand_obj obj(0);
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(obj.next());
    std::cout << obj.color << " " << obj.x << std::endl;
    BOOST_REQUIRE(obj.color == RED || obj.color == GREEN || obj.color == BLUE);
    BOOST_REQUIRE(obj.color == obj.x);
  }
}

BOOST_AUTO_TEST_CASE( t_rand_enum_standalone )
{
  BOOST_CHECK_THROW ( randv<color_enum> color(0), std::runtime_error );
}

class tall_rand_enum_obj : public rand_obj {
public:
  randv<football_enum> player;

  tall_rand_enum_obj(rand_obj* parent = 0) : rand_obj(parent), player(this) {
    constraint(player() == GK && player() != CF);
  }
};

class tall_rand_enum_obj_gt : public rand_obj {
public:
  randv<football_enum> player;

  tall_rand_enum_obj_gt(rand_obj* parent = 0) : rand_obj(parent), player(this) {
    constraint(player() > AM);
  }
};

BOOST_AUTO_TEST_CASE( enum_no_overflow )
{
  tall_rand_enum_obj obj(0);
  BOOST_REQUIRE(obj.next());

  BOOST_REQUIRE_EQUAL(obj.player, GK);
}

BOOST_AUTO_TEST_CASE( enum_gt )
{
  tall_rand_enum_obj_gt obj(0);

  for (int i = 0; i<100; ++i) {
    BOOST_REQUIRE(obj.next());
    BOOST_REQUIRE_GT(obj.player, AM);
    BOOST_REQUIRE(obj.player == LW || obj.player == SS || obj.player == RW || obj.player == CF );
  }
}

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
  BOOST_REQUIRE(it.a + it.b + it.c <= 200);
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

class obj : public rand_obj {
public:
  obj(rand_obj* parent) : rand_obj(parent), a(this), b(this), c(this), d(this), e(this), f(this) {
    a.range(-20, -10);
    b.range(10, 20);
    c.range(-20, -10);
    d.range(10, 20);
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
  obj2(rand_obj* parent) : rand_obj(parent), g(this), h(this), i(this), j(this), k(this), l(this) {
    g.range(-20, -10);
    h.range(10, 20);
    i.range(-20, -10);
    j.range(10, 20);
  }
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

BOOST_AUTO_TEST_CASE( t5 )
{
  obj it(0);
  for (int i = 0; i < 20; i++) {
    it.next();
    BOOST_REQUIRE(-20 <= it.a && it.a <= -10);
    BOOST_REQUIRE(10 <= it.b && it.b <= 20);
    BOOST_REQUIRE(-20 <= it.c && it.c <= -10);
    BOOST_REQUIRE(10 <= it.d && it.d <= 20);
    BOOST_REQUIRE('a' <= it.e && it.e <= 'z');
    BOOST_REQUIRE('A' <= it.f && it.f <= 'Z');
  }

  obj1 it1(0);
  for (int i = 0; i < 20; i++) {
    it1.next();
    BOOST_REQUIRE(-20 <= it1.a && it1.a <= -10);
    BOOST_REQUIRE(10 <= it1.b && it1.b <= 20);
    BOOST_REQUIRE(-20 <= it1.c && it1.c <= -10);
    BOOST_REQUIRE(10 <= it1.d && it1.d <= 20);
    BOOST_REQUIRE('a' <= it1.f && it1.f <= 'z');
    BOOST_REQUIRE('A' <= it1.e && it1.e <= 'Z');
  }

  obj2 it2(0);
  for (int i = 0; i < 20; i++) {
    it2.next();
    BOOST_REQUIRE(-20 <= it2.g && it2.g <= -10);
    BOOST_REQUIRE(10 <= it2.h && it2.h <= 20);
    BOOST_REQUIRE(-20 <= it2.i && it2.i <= -10);
    BOOST_REQUIRE(10 <= it2.j && it2.j <= 20);
    BOOST_REQUIRE(-20 <= it2.l.a && it2.l.a <= -10);
    BOOST_REQUIRE(10 <= it2.l.b && it2.l.b <= 20);
    BOOST_REQUIRE(-20 <= it2.l.c && it2.l.c <= -10);
    BOOST_REQUIRE(10 <= it2.l.d && it2.l.d <= 20);
    BOOST_REQUIRE('a' <= it2.l.f && it2.l.f <= 'z');
    BOOST_REQUIRE('A' <= it2.l.e && it2.l.e <= 'Z');
  }
}

struct Item1 : public rand_obj {
  Item1() : x(this), pivot(0) {
    constraint("c1", x() * x() >= 24);
    constraint("c2", x() <= reference(pivot));
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

  randv<unsigned> x;
  int pivot;
};

BOOST_AUTO_TEST_CASE ( binary_search_test )
{
  Item1 it;
  it.next();
  BOOST_REQUIRE_EQUAL(it.x, 5);
}

struct Item2 : public rand_obj {
  Item2() : i(), address(this), data(this) {
    constraint(address() % 4 == 0);
    constraint(address() <= 1000u);
    constraint(data().size() == 4);
    constraint.foreach(data, i, -50 <= data()[i] && data()[i] <= 50);
    constraint.foreach(data, i, data()[i - 1] <= data()[i]);
  }

  placeholder i;
  randv<unsigned> address;
  rand_vec<int> data;
};

BOOST_AUTO_TEST_CASE ( item_with_vector )
{
  Item2 it;
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(it.next());
    BOOST_REQUIRE(it.data.size() == 4);
    std::cout << "@" << it.address << ": " << it.data[0] << " " << it.data[1] << " " << it.data[2] << " " << it.data[3] << std::endl;
    BOOST_REQUIRE(it.address % 4 == 0);
    BOOST_REQUIRE(it.address <= 1000);
    for (uint i = 0; i < it.data.size(); i++) {
      BOOST_REQUIRE(-50 <= it.data[i] && it.data[i] <= 50);
      if (i > 0) BOOST_REQUIRE(it.data[i - 1] <= it.data[i]);
    }
  }
}

class Constraint_base : public Generator {
  public:
    Constraint_base()
      : Generator()
      , constraint(*this)
      , soft_constraint( constraint )
    {}

  protected:
    Generator & constraint;
    Soft_Generator soft_constraint;
};

class Constraint1 : public Constraint_base {
  public:
    Variable<unsigned> x;

    Constraint1 () : Constraint_base() {
      constraint( x<10 );
    }
};

class Constraint2 : public Constraint1 {
  public:
    Constraint2 () : Constraint1() {
      constraint( x>6 );
    }
};

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

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
