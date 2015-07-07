#define BOOST_TEST_MODULE experimental

#include <crave/experimental/Experimental.hpp>

struct Context_Fixture {
  Context_Fixture() {}
};

#include "test_CoverageSampling.cpp"
#include "test_ObjectHierarchy.cpp"
#include "../src/crave/experimental/Variable.hpp"
#include "../src/crave/experimental/Constraint.hpp"

class Item : public crv_sequence_item {
 public:
  Item() : crv_sequence_item(), sumName("sum"),anonymName("anonym"),productName("product"),xName("x")  {
      this->children_.push_back(&a);
      this->children_.push_back(&b);
      sum = new crv_constraint(sumName);
      sum->operator() (a() + b() == 4);
      this->children_.push_back(sum);
      
      product = new crv_constraint(productName);
      product->operator() (a() * b() == 4);
      this->children_.push_back(product);
      
      anonym = new crv_constraint(anonymName);
      anonym->operator() (a() < 10);
      anonym->operator() (b() < 10);
      this->children_.push_back(anonym);
      
      x = new crv_constraint(xName);
      x->operator() (a() != 2);
      this->children_.push_back(x);
  }
  crv_variable<uint> a;
  crv_variable<uint> b;
  
  crv_object_name sumName;
  crv_object_name anonymName;
  crv_object_name productName;
  crv_object_name xName;
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