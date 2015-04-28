#include <boost/test/unit_test.hpp>

#include <crave/experimental/Object.hpp>

#include <vector>

using namespace crave;

BOOST_FIXTURE_TEST_SUITE(ObjectHierarchy, Context_Fixture)

struct base_object : crv_object {
  base_object(crv_object_name ) { }
};

struct random_object1 : crv_object {
  base_object v1;
  base_object v2;
  
  random_object1(crv_object_name);
};

random_object1::random_object1(crv_object_name n) : v1("v1"), v2("v2") {}

struct random_object2 : crv_object {
  random_object1 obj;
  base_object v3;
  
  random_object2(crv_object_name);
};

random_object2::random_object2(crv_object_name n) : obj("obj"), v3("v3") {}

struct random_object3 : crv_object {
  random_object1 obj;
  base_object v3;
  random_object1* obj1;
  
  random_object3(crv_object_name);
};

random_object3::random_object3(crv_object_name n) : obj("obj"), v3("v3") {
  obj1 = new random_object1("obj1_ptr");  
  random_object2 tmp("tmp");
}

BOOST_AUTO_TEST_CASE(basic_test) {
  random_object1 o1("o1");
  random_object2 o2("o2");
  random_object3 o3("o3");
  BOOST_CHECK_EQUAL(crv_object::count(), 17);
  BOOST_CHECK(crv_object::find("root") == nullptr);
  BOOST_CHECK(crv_object::find("o3.obj") == nullptr);
  BOOST_CHECK_EQUAL(crv_object::find("root.o3.obj"), &o3.obj);
}

BOOST_AUTO_TEST_SUITE_END()  // ObjectHierarchy

//  vim: ft=cpp:ts=2:sw=2:expandtab
