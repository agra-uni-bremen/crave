#include <boost/test/unit_test.hpp>

#include <crave/ir/UserConstraint.hpp>

//using namespace std;
using namespace crave;

BOOST_FIXTURE_TEST_SUITE(UserConstraint, Context_Fixture )

BOOST_AUTO_TEST_CASE( constraint_partitioning )
{
  randv<unsigned> a, b, c, d, e;
  ConstraintPartitioner cp;
  ConstraintManager cm1, cm2;
  Context ctx(crave::variables);
  cm1.makeConstraint(a() > b(), ctx);
  cm1.makeConstraint(c() > d(), ctx);
  cm1.makeConstraint(e() > 1, ctx);
  cm2.makeConstraint(a() != e(), ctx);
  
  cp.reset();
  cp.mergeConstraints(cm1);
  cp.partition();
  BOOST_CHECK_EQUAL(cp.getPartitions().size(), 3);
  
  cp.reset();
  cp.mergeConstraints(cm1);
  cp.mergeConstraints(cm2);
  cp.partition();
  BOOST_CHECK_EQUAL(cp.getPartitions().size(), 2);
}

BOOST_AUTO_TEST_SUITE_END() // Syntax

//  vim: ft=cpp:ts=2:sw=2:expandtab
