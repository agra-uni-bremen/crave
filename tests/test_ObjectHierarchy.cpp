#include <boost/test/unit_test.hpp>

#include <crave/experimental/Object.hpp>

#include <vector>

using namespace crave;

BOOST_FIXTURE_TEST_SUITE(ObjectHierarchy, Context_Fixture)

struct base_object : crv_object {
  base_object(crv_object_name) {}
};

struct random_object1 : crv_object {
  base_object v1{"v1"};
  base_object v2{"v2"};
  random_object1(crv_object_name) {}
};

struct random_object2 : crv_object {
  random_object1 obj{"obj"};
  base_object v3{"v3"};

  random_object2(crv_object_name) {}
};

struct random_object3 : crv_object {
  random_object1 obj{"obj"};
  base_object v3{"v3"};

  random_object1* obj1;

  random_object3(crv_object_name);
};

random_object3::random_object3(crv_object_name) {
  obj1 = new random_object1{"obj1_ptr"};
  random_object2 tmp{"tmp"};
}

BOOST_AUTO_TEST_CASE(basic_test) {
  random_object1 o1{"o1"};
  random_object2 o2{"o2"};
  random_object3 o3{"o3"};
  crv_object::root()->print_object_hierarchy();
  BOOST_CHECK_EQUAL(crv_object::count(), 16);

  BOOST_CHECK(crv_object::find("root") == nullptr);

  BOOST_CHECK_EQUAL(crv_object::find("o1"), &o1);
  BOOST_CHECK_EQUAL(crv_object::find("o1.v1"), &o1.v1);
  BOOST_CHECK_EQUAL(crv_object::find("o1.v2"), &o1.v2);

  BOOST_CHECK_EQUAL(crv_object::find("o2"), &o2);
  BOOST_CHECK_EQUAL(crv_object::find("o2.obj"), &o2.obj);
  BOOST_CHECK_EQUAL(crv_object::find("o2.v3"), &o2.v3);

  BOOST_CHECK_EQUAL(crv_object::find("o3"), &o3);
  BOOST_CHECK_EQUAL(crv_object::find("o3.obj"), &o3.obj);
  BOOST_CHECK_EQUAL(crv_object::find("o3.v3"), &o3.v3);
  BOOST_CHECK_EQUAL(crv_object::find("o3.obj1_ptr"), o3.obj1);
}

struct random_object4 : public random_object2 {
  random_object1 other_obj{"other_obj"};

  random_object4(crv_object_name name) : random_object2(name) {}
};

struct random_object5 : public random_object4 {
  random_object5(crv_object_name name) : random_object4(name) {}
};

BOOST_AUTO_TEST_CASE(inheritance_test) {
  random_object5 o5{"o5"};
  random_object3 o3{"o3"};

  BOOST_CHECK_EQUAL(crv_object::find("o5"), &o5);
  BOOST_CHECK_EQUAL(crv_object::find("o5.obj"), &o5.obj);
  BOOST_CHECK_EQUAL(crv_object::find("o5.v3"), &o5.v3);
  BOOST_CHECK_EQUAL(crv_object::find("o5.other_obj"), &o5.other_obj);

  BOOST_CHECK_EQUAL(crv_object::find("o3"), &o3);
  BOOST_CHECK_EQUAL(crv_object::find("o3.obj"), &o3.obj);
  BOOST_CHECK_EQUAL(crv_object::find("o3.v3"), &o3.v3);
  BOOST_CHECK_EQUAL(crv_object::find("o3.obj1_ptr"), o3.obj1);
}

BOOST_AUTO_TEST_SUITE_END()  // ObjectHierarchy

//  vim: ft=cpp:ts=2:sw=2:expandtab
