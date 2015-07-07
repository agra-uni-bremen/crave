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

class Item : public crv_sequence_item {
 public:
  Item() : crv_sequence_item()  {
      this->children_.push_back(&a);
      this->children_.push_back(&b);
      sum = new crv_constraint("sum");
      sum->operator() (a() + b() == 4);
      this->children_.push_back(sum);
      
      product = new crv_constraint("product");
      product->operator() (a() * b() == 4);
      this->children_.push_back(product);
      
      anonym = new crv_constraint("anonym");
      anonym->operator() (a() < 10);
      anonym->operator() (b() < 10);
      this->children_.push_back(anonym);
      
      x = new crv_constraint("x");
      x->operator() (a() != 2);
      this->children_.push_back(x);
  }
  crv_variable<uint> a;
  crv_variable<uint> b;
  
  crv_constraint *sum;
  crv_constraint *anonym;
  crv_constraint *product;
  crv_constraint *x;
  friend ostream& operator<<(ostream& os, const Item& it) {
    os << it.a << " " << it.b;
    return os;
  }
};

BOOST_AUTO_TEST_CASE(t1) {
  Item it;

  
  BOOST_REQUIRE(!it.randomize());
  BOOST_REQUIRE(it.sum->active());
  BOOST_REQUIRE(it.product->active());
  BOOST_REQUIRE(it.x->active());

  
  it.sum->deactivate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(!it.sum->active());
  BOOST_REQUIRE(it.product->active());
  BOOST_REQUIRE(it.x->active());
  std::cout << it << std::endl;

 it.product->deactivate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(!it.sum->active());
  BOOST_REQUIRE(!it.product->active());
  BOOST_REQUIRE(it.x->active());
  std::cout << it << std::endl;

  it.sum->activate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(it.sum->active());
  BOOST_REQUIRE(!it.product->active());
  BOOST_REQUIRE(it.x->active());
  std::cout << it << std::endl;

  it.product->activate();
  BOOST_REQUIRE(!it.randomize());
  BOOST_REQUIRE(it.sum->active());
  BOOST_REQUIRE(it.product->active());
  BOOST_REQUIRE(it.x->active());

  it.x->deactivate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(it.sum->active());
  BOOST_REQUIRE(it.product->active());
  BOOST_REQUIRE(!it.x->active());

  std::cout << it << std::endl;
  BOOST_REQUIRE(it.a == 2 && it.b == 2);
}
 