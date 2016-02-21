#include <boost/test/unit_test.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

using boost::format;
using namespace crave;

CRAVE_BETTER_ENUM(color_enum, RED,GREEN,BLUE);
CRAVE_BETTER_ENUM(football_enum,
  GK,   // Goalkeeper
  SW,   // Sweeper
  LWB,  // Left-Wing-Back
  LB,   // Left-Back
  LCB,  // Left-Centre-Back
  RCB,  // Right-Centre-Back
  RB,   // Right-Back
  RWB,  // Right-Wing-Back
  DM,   // Defensive Midfielder
  LM,   // Left Wide Midfielder
  CM,   // Centre Midfielder
  RM,   // Right Wide Midfielder
  AM,   // Attacking Midfielder
  LW,   // Left Winger (Striker)
  SS,   // Secondary Striker
  RW,   // Right Winger
  CF    // Centre Striker
);

BOOST_FIXTURE_TEST_SUITE(SequenceItem_t, Context_Fixture)

class my_rand_obj : public crv_sequence_item {
 public:
  crv_variable<color_enum> color;
  crv_variable<int> x;
  crv_constraint constraint { color() == x() };

  my_rand_obj(crv_object_name){ }
};

BOOST_AUTO_TEST_CASE(t_rand_enum) {
  my_rand_obj obj("obj");
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(obj.randomize());
    std::cout << obj.color << " " << obj.x << std::endl;
    BOOST_REQUIRE(obj.color == color_enum::RED || obj.color == color_enum::GREEN || obj.color == color_enum::BLUE);
    BOOST_REQUIRE(obj.color == obj.x);
  }
}

class tall_rand_enum_obj : public crv_sequence_item {
 public:
  crv_variable<football_enum> player;
  crv_constraint constraint { player() == football_enum::GK && player() != football_enum::CF };

  tall_rand_enum_obj(crv_object_name) { }
};

class tall_rand_enum_obj_gt : public crv_sequence_item {
 public:
  crv_variable<football_enum> player;
  crv_constraint constraint { player() > football_enum::AM };
  tall_rand_enum_obj_gt(crv_object_name) { }
};

BOOST_AUTO_TEST_CASE(enum_no_overflow) {
  tall_rand_enum_obj obj("obj");
  BOOST_REQUIRE(obj.randomize());

  BOOST_REQUIRE_EQUAL(obj.player, football_enum::GK);
}

BOOST_AUTO_TEST_CASE(enum_gt) {
  tall_rand_enum_obj_gt obj("obj");

  for (int i = 0; i < 100; ++i) {
    BOOST_REQUIRE(obj.randomize());
    BOOST_REQUIRE_GT(obj.player, football_enum::AM);
    BOOST_REQUIRE(obj.player == football_enum::LW || obj.player == football_enum::SS || obj.player == football_enum::RW || obj.player == football_enum::CF);
  }
}

class item : public crv_sequence_item {
 public:
  item(crv_object_name){ }

 public:
  crv_variable<int> a;
  crv_variable<int> b;
  crv_variable<int> c;
  crv_constraint constraint { a() + b() == c() };
};

class item1 : public item {
 public:
  crv_constraint constraint { 10 <= a(), a() <= 20, a() + b() + c() <= 200 };
  item1(crv_object_name name) : item(name){ }
};

class item2 : public item1 {
 public:
  crv_variable<int> d;
  crv_constraint constraint { a() + b() + c() == 100 };
  item2(crv_object_name name) : item1(name){ }
};

BOOST_AUTO_TEST_CASE(t2) {
  item it("it");
  it.randomize();
  std::cout << it.a << " " << it.b << " " << it.c << std::endl;
  BOOST_REQUIRE(it.a + it.b == it.c);
}

BOOST_AUTO_TEST_CASE(t3) {
  item1 it("it");
  it.randomize();
  std::cout << it.a << " " << it.b << " " << it.c << std::endl;
  BOOST_REQUIRE(it.a + it.b == it.c);
  BOOST_REQUIRE(10 <= it.a && it.a <= 20);
  BOOST_REQUIRE(it.a + it.b + it.c <= 200);
}

BOOST_AUTO_TEST_CASE(t4) {
  item2 it("it");
  it.randomize();
  std::cout << it.a << " " << it.b << " " << it.c << std::endl;
  BOOST_REQUIRE(it.a + it.b == it.c);
  BOOST_REQUIRE(10 <= it.a && it.a <= 20);
  BOOST_REQUIRE(it.a + it.b + it.c == 100);
}

class obj : public crv_sequence_item {
 public:
  obj(crv_object_name){ }

  crv_variable<int> a;
  crv_variable<unsigned int> b;
  crv_variable<short> c;
  crv_variable<unsigned short> d;
  crv_variable<char> e;
  crv_variable<unsigned char> f;

  crv_constraint constraint {dist(a(), distribution<int>::simple_range(-20, -10)),
    dist(b(), distribution<unsigned int>::simple_range(10, 20)),
    dist(c(), distribution<short>::simple_range(-20, -10)),
    dist(d(), distribution<unsigned short>::simple_range(10, 20))}; 
  crv_constraint e_con {dist(e(), distribution<char>::simple_range('a', 'z'))};
  crv_constraint f_con {dist(f(), distribution<unsigned char>::simple_range('A', 'Z'))};

  friend ostream& operator<<(ostream& os, const obj& o) {
    os << "(" << o.a << " " << o.b << " " << o.c << " " << o.d << " " << o.e << " " << o.f << ")";
    return os;
  }
};

class obj1 : public obj {
 public:
  crv_constraint constraint {
      dist(e(), distribution<char>::simple_range('A', 'Z')), 
      dist(f(), distribution<unsigned char>::simple_range('a', 'z'))};
  
  obj1(crv_object_name name) : obj(name){
    e_con.deactivate();
    f_con.deactivate();
  }
};

class obj2 : public obj1 {
 public:
  obj2(crv_object_name name) : l("l") , obj1(name) { }

  crv_variable<long> g;
  crv_variable<unsigned long> h;
  crv_variable<long long> i;
  crv_variable<unsigned long long> j;
  crv_variable<bool> k;
  obj1 l;

  crv_constraint constraint {dist(g(), distribution<long>::simple_range(-20, -10)),
   dist(h(), distribution<unsigned long>::simple_range(10, 20)),
   dist(i(), distribution<long long>::simple_range(-20, -10)),
   dist(j(), distribution<unsigned long long>::simple_range(10, 20))};

  friend ostream& operator<<(ostream& os, const obj2& o1) {
    os << o1.l << " " << o1.g << " " << o1.h << " " << o1.i << " " << o1.j << " " << o1.k;
    return os;
  }
};

BOOST_AUTO_TEST_CASE(t5) {
  obj it("obj");
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(it.randomize());
    BOOST_REQUIRE(-20 <= it.a && it.a <= -10);
    BOOST_REQUIRE(10 <= it.b && it.b <= 20);
    BOOST_REQUIRE(-20 <= it.c && it.c <= -10);
    BOOST_REQUIRE(10 <= it.d && it.d <= 20);
    BOOST_REQUIRE('a' <= it.e && it.e <= 'z');
    BOOST_REQUIRE('A' <= it.f && it.f <= 'Z');
  }

  obj1 it1("obj");
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(it1.randomize());
    BOOST_REQUIRE(-20 <= it1.a && it1.a <= -10);
    BOOST_REQUIRE(10 <= it1.b && it1.b <= 20);
    BOOST_REQUIRE(-20 <= it1.c && it1.c <= -10);
    BOOST_REQUIRE(10 <= it1.d && it1.d <= 20);
    BOOST_REQUIRE('a' <= it1.f && it1.f <= 'z');
    BOOST_REQUIRE('A' <= it1.e && it1.e <= 'Z');
  }
  
  obj2 it2("obj");
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(it2.randomize());
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

struct Item1 : public crv_sequence_item {
  
  Item1(crv_object_name) : pivot(0) { }

  bool randomize() override {
    int lower = 0;
    int upper = 100;
    while (lower < upper) {
      std::cout << lower << " " << upper << std::endl;
      pivot = (upper + lower) / 2;
      if (crv_sequence_item::randomize())
        upper = x;
      else
        lower = pivot + 1;
    }
    x = upper;
    return true;
  }

  crv_variable<unsigned> x;
  crv_constraint c1 {x() * x() >= 24};
  crv_constraint c2 {x() <= reference(pivot)};
  int pivot;
};

BOOST_AUTO_TEST_CASE(binary_search_test) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Item1 it("it");
  it.randomize();
  BOOST_REQUIRE_EQUAL(it.x, 5);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

struct Item2 : public crv_sequence_item {
  Item2(crv_object_name) { }
    
  crv_variable<unsigned> address;
  crv_vector<int> data;
  crv_constraint constraint={
    address() % 4 == 0,
    address() <= 1000u,
    data().size() == 4,
    foreach(data(), -50 <= data()[_i] && data()[_i] <= 50),
    foreach(data(), data()[_i - 1] <= data()[_i]),
  };
};

BOOST_AUTO_TEST_CASE(item_with_vector) {
  Item2 it("it");
  for (int i = 0; i < 20; i++) {
    BOOST_REQUIRE(it.randomize());
    BOOST_REQUIRE_EQUAL(it.data.size(), 4);
    std::cout << "@" << it.address << ": " << it.data[0] << " " << it.data[1] << " " << it.data[2] << " " << it.data[3]
              << std::endl;
    BOOST_REQUIRE_EQUAL(it.address % 4, 0);
    BOOST_REQUIRE_LE(it.address, 1000);
    for (uint i = 0; i < it.data.size(); i++) {
      BOOST_REQUIRE_LE(-50, it.data[i]);
      BOOST_REQUIRE_LE(it.data[i], 50);
      if (i > 0) BOOST_REQUIRE_LE(it.data[i - 1], it.data[i]);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()  // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
