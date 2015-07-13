#define BOOST_TEST_MODULE experimental

#include <crave/experimental/Experimental.hpp>

struct Context_Fixture {
  Context_Fixture() {}
};

#include "test_CoverageSampling.cpp"
#include "test_ObjectHierarchy.cpp"

#include "../src/crave/experimental/Variable.hpp"
#include "../src/crave/experimental/Coverage.hpp"
#include "../src/crave/experimental/Constraint.hpp"
#include <boost/test/unit_test.hpp>

using crave::crv_sequence_item;
using crave::crv_variable;
using crave::crv_object_name;
using crave::crv_constraint;

struct Item : public crv_sequence_item{
      crv_variable<unsigned int> a { "a" };
      crv_variable<unsigned int> b { "b" };
      
      crv_constraint sum {"sum"};
      crv_constraint product {"product"};
      crv_constraint anonym {"anonym"};
      crv_constraint x {"x"};
      
      Item(crv_object_name)
      {
          sum = {a() + b() == 4};
          product = {a() * b() == 4};
          anonym = {a() < 10,b() < 10};
          x = {a() != 2};
          sum.activate();
          product.activate();
          anonym.activate();
          x.activate();
      }
      friend ostream& operator<<(ostream& os, const Item& it) {
      os << it.a << " " << it.b;
    return os;
  }
};


BOOST_AUTO_TEST_CASE(t1) {
  Item it("Item");

  BOOST_REQUIRE(!it.randomize());
  BOOST_REQUIRE(it.sum.active());
  BOOST_REQUIRE(it.product.active());
  BOOST_REQUIRE(it.x.active());

  it.sum.deactivate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(!it.sum.active());
  BOOST_REQUIRE(it.product.active());
  BOOST_REQUIRE(it.x.active());
  std::cout << it << std::endl;

 it.product.deactivate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(!it.sum.active());
  BOOST_REQUIRE(!it.product.active());
  BOOST_REQUIRE(it.x.active());
  std::cout << it << std::endl;

  it.sum.activate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(it.sum.active());
  BOOST_REQUIRE(!it.product.active());
  BOOST_REQUIRE(it.x.active());
  std::cout << it << std::endl;

  it.product.activate();
  BOOST_REQUIRE(!it.randomize());
  BOOST_REQUIRE(it.sum.active());
  BOOST_REQUIRE(it.product.active());
  BOOST_REQUIRE(it.x.active());

  it.x.deactivate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(it.sum.active());
  BOOST_REQUIRE(it.product.active());
  BOOST_REQUIRE(!it.x.active());

  std::cout << it << std::endl;
  BOOST_REQUIRE(it.a == 2 && it.b == 2);
}
 