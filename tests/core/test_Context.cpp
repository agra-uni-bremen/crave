#include <crave/ConstrainedRandom.hpp>
#undef CHECK  // Conflicts with doctest

#include <doctest/doctest.h>

#include <iostream>
#include <set>

using namespace crave;

TEST_CASE("multiple_solver_instances") {
  Generator gen1, gen2;
  Variable<int> r1, r2;
  gen1(gen1.write_var(r1) < 6);
  gen2(gen2.write_var(r2) < 6);
  REQUIRE(gen1.next());
  REQUIRE(gen2.next());
}

TEST_CASE("constants") {
  Variable<unsigned> x;
  Generator gen;
  gen(gen.write_var(x) == 135421);
  gen();
  REQUIRE_EQ(gen[x], 135421);
}

TEST_CASE("boolean") {
  Variable<bool> b;

  Generator gen;
  gen(gen.write_var(b) == gen.write_var(b))();  // create a new assignment

  bool b1 = gen[b];
  gen(gen.write_var(b) != gen.write_var(b1))();
  bool b2 = gen[b];

  // FIXME:
  // REQUIRE_THROWS_AS(gen(gen.write_var(b) != gen.write_var(b2))(), std::runtime_error);
}

TEST_CASE("by_reference") {
  unsigned b = 0;
  Variable<unsigned> a;

  Generator gen;
  gen(gen.write_var(a) == gen.read_var(b));

  while (gen.next()) {
    unsigned av = gen[a];
    REQUIRE_EQ(av, b);
    ++b;
    if (b > 10) break;
  }

  REQUIRE_GT(b, 10);
}

// temporaly fix a variable to a certain value using the assign operator
// FIXME:
/*TEST_CASE("named_reference") {
  unsigned bv = 0;
  Variable<unsigned> a, c;
  ReadReference<unsigned> b(bv);

  Generator gen;
  gen(gen.write_var(a) == b);
  gen(gen.write_var(c) != b);

  while (gen.next()) {
    unsigned av = gen[a];
    unsigned cv = gen[c];
    REQUIRE_EQ(av, bv);
    REQUIRE_NE(cv, bv);
    ++bv;
    if (bv > 10) break;
  }

  REQUIRE_GT(bv, 10);
}*/

TEST_CASE("soft_constraint_t") {
  Generator gen;
  Variable<int> r;
  gen(gen.write_var(r) < 6);
  gen.soft(gen.write_var(r) == 2);

  REQUIRE(gen.next());
  REQUIRE_EQ(gen[r], 2);

  gen.soft(gen.write_var(r) == 3);

  REQUIRE(gen.next());
}

TEST_CASE("Variable_test") {
  Generator gen;
  Variable<int> a;
  Variable<int> b;
  auto av = gen.write_var(a), bv = gen.write_var(b);
  gen(4 <= av && av <= 6)(9 <= av + bv && av + bv <= 11)(bv % 2 == 0);
  int count = 0;
  while (gen.next()) {
    ++count;
    gen(av != gen[a] || bv != gen[b]);
    REQUIRE_LE(count, 10);
  }
  REQUIRE_EQ(count, 4);
}

TEST_CASE("Variable_var_ref_mixed_test") {
  Generator gen;
  Variable<int> a;
  Variable<int> b;
  auto av = gen.write_var(a), bv = gen.write_var(b);
  gen(4 <= av && av <= 6)(9 <= av + bv && av + bv <= 11)(bv % 2 == 0);
  unsigned count = 0;
  while (gen.next()) {
    ++count;
    std::cout << "result: a = " << gen[a] << ", b = " << gen[b] << std::endl;
    gen(av != gen[a] || bv != gen[b]);

    REQUIRE_LE(count, 10);
  }
  REQUIRE_EQ(count, 4);
}

TEST_CASE("alu") {
  Variable<unsigned> op;
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator gen;
  auto opv = gen.write_var(op), av = gen.write_var(a), bv = gen.write_var(b);
  gen(av < 16)(bv < 16)(opv < 4)  // 4 opcodes
      (opv != 0 || av + bv < 16)  // no add overflow
      (opv != 1 || av > bv)       // no sub underflow
      (opv != 2 || av * bv < 16)  // no m overflow
      (opv != 3 || bv != 0)       // div valid
      ;
  gen();  // create a new assignment

  // printf("result: op=%d, a=%d, b=%d\n", gen[op], gen[a], gen[b]);
}

TEST_CASE("alu_enum") {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<unsigned> op;
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator gen;
  auto opv = gen.write_var(op), av = gen.write_var(a), bv = gen.write_var(b);
  gen(av < 8u)(bv < 8u)(opv < 4u)  // 4 opcodes
      (opv != 0u || av + bv < 8u)  // no add overflow
      (opv != 1u || av > bv)       // no sub underflow
      (opv != 2u || av * bv < 8u)  // no m overflow
      (opv != 3u || bv != 0u)      // div valid
      ;

  int count = 0;
  for (int a = 0; a < 8; a++)
    for (int b = 0; b < 8; b++) {
      if (a + b < 8) count++;  // op == 0
      if (a > b) count++;      // op == 1
      if (a * b < 8) count++;  // op == 2
      if (b != 0) count++;     // op == 3;
    }

  while (gen.next()) {
    --count;
    gen(opv != gen[op] || av != gen[a] || bv != gen[b]);

    REQUIRE_GE(count, 0);
  }
  REQUIRE_EQ(count, 0);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

TEST_CASE("pythagoras") {
  Variable<unsigned long long> a;
  Variable<unsigned long long> b;
  Variable<unsigned long long> c;

  Generator gen;
  auto av = gen.write_var(a), bv = gen.write_var(b), cv = gen.write_var(c);
  gen(av * av + bv * bv == cv * cv)();  // create a new assignment

  {
    unsigned long long av = gen[a];
    unsigned long long bv = gen[b];
    unsigned long long cv = gen[c];

    REQUIRE_EQ(av * av + bv * bv, cv * cv);
  }
}

TEST_CASE("negative_var") {
  Variable<int> a;
  Variable<int> b;

  Generator gen;
  auto av = gen.write_var(a), bv = gen.write_var(b);
  gen(av + bv <= 120)(av > 120);

  gen();

  std::cout << "result: a=" << gen[a] << ", b=" << gen[b] << std::endl;

  REQUIRE(gen[a] + gen[b] <= 120);
}

TEST_CASE("signed_less_zero") {
  Variable<int> a;

  Generator gen;
  auto av = gen.write_var(a);
  gen(av < 0);

  gen();

  std::cout << "result: a=" << gen[a] << std::endl;

  REQUIRE(gen[a] < 0);
}

TEST_CASE("mixed_bv_width_1") {
  Variable<unsigned long> a;
  Variable<unsigned short> b;

  Generator gen;
  auto av = gen.write_var(a), bv = gen.write_var(b);
  gen(av + bv >= 120);

  REQUIRE(gen.next());

  std::cout << "result: a=" << gen[a] << ", b=" << gen[b] << std::endl;

  REQUIRE(gen[a] + gen[b] >= 120);
}

TEST_CASE("mixed_bv_width_2") {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<signed char> a;
  Generator gen;
  auto av = gen.write_var(a);
  gen(av < 10);

  std::set<signed char> generated;
  for (int iterations = 0; gen.next(); ++iterations) {
    generated.insert(gen[a]);
    gen(av != gen[a]);

    REQUIRE_LT(iterations, 150);
  }

  REQUIRE_EQ(generated.size(), 138);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

TEST_CASE("mixed_bv_width_3") {
  Variable<short> a;
  Generator gen;
  auto av = gen.write_var(a);
  gen(av < 10);
  gen(av > -10);

  std::set<short> generated;
  for (unsigned iterations = 0; gen.next(); ++iterations) {
    generated.insert(gen[a]);
    gen(av != gen[a]);

    REQUIRE_LT(iterations, 20);
  }

  REQUIRE_EQ(generated.size(), 19);
}

TEST_CASE("mixed_bv_width_4") {
  Variable<int> a;
  Generator gen;
  auto av = gen.write_var(a);
  gen(av < (signed char)10);
  gen(av > (short)-10);

  std::set<int> generated;
  for (unsigned iterations = 0; gen.next(); ++iterations) {
    generated.insert(gen[a]);
    gen(av != gen[a]);

    REQUIRE_LT(iterations, 20);
  }

  REQUIRE_EQ(generated.size(), 19);
}

TEST_CASE("mixed_bv_width_5") {
  Variable<short> a;
  Variable<signed char> b;
  Generator gen;
  auto av = gen.write_var(a), bv = gen.write_var(b);
  gen(-3 <= av && av <= 3);
  gen(-3 <= bv && bv <= 3);
  gen((-2 <= av + bv) && (av + bv <= 2));

  int cnt = 0;
  while (gen.next()) {
    cnt++;
    gen((av != gen[a]) || (bv != gen[b]));

    REQUIRE_LT(cnt, 300);
  }

  int cnt1 = 0;
  for (short i = -3; i <= 3; i++)
    for (signed char j = -3; j <= 3; j++)
      if ((-2 <= i + j) && (i + j <= 2)) cnt1++;

  REQUIRE_EQ(cnt, cnt1);
}

TEST_CASE("mixed_bv_width_6") {
  Variable<short> a;
  Variable<signed char> b;

  Generator gen;
  auto av = gen.write_var(a), bv = gen.write_var(b);
  gen(-3 <= av && av <= 3)(-3 <= bv && bv <= 3)(av * bv % 6 == 0);

  int cnt = 0;
  for (int i = -3; i <= 3; i++)
    for (int j = -3; j <= 3; j++)
      if (i * j % 6 == 0) cnt++;

  int cnt1 = 0;
  while (gen.next()) {
    cnt1++;
    REQUIRE_EQ(gen[a] * gen[b] % 6, 0);
    gen(av != gen[a] || bv != gen[b]);
  }

  REQUIRE_EQ(cnt, cnt1);
}

//  vim: ft=cpp:ts=2:sw=2:expandtab
