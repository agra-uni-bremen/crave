#include <systemc.h>

#include <chrono>
#include <crave/ConstrainedRandom.hpp>
#include <crave/SystemC.hpp>

using crave::rand_obj;
using crave::randv;
using sc_dt::sc_bv;
using sc_dt::sc_uint;

using namespace std::chrono;

/**
 * ALU:
 * complete enumerated there are:
 * ADD 0x0: 136
 * SUB 0x1: 136
 * MUL 0x2:  76
 * DIV 0x3: 240
 * valid assignments.
 */
struct ALU4 : public rand_obj {
  randv<sc_bv<2> > op;
  randv<sc_uint<4> > a, b;

  ALU4(rand_obj* parent = nullptr) : rand_obj(parent), op(this), a(this), b(this) {
    auto opv = constraint.write_var(op);
    auto av = constraint.write_var(a);
    auto bv = constraint.write_var(b);
    constraint((opv != (unsigned char)0x0) || ((unsigned char)15 >= av + bv));
    constraint((opv != (unsigned char)0x1) || (((unsigned char)15 >= av - bv) && (bv <= av)));
    constraint((opv != (unsigned char)0x2) || ((unsigned char)15 >= av * bv));
    constraint((opv != (unsigned char)0x3) || (bv != (unsigned char)0));
  }

  friend std::ostream& operator<<(std::ostream& o, ALU4 const& alu) {
    o << alu.op << ' ' << alu.a << ' ' << alu.b;
    return o;
  }
};

int sc_main(int argc, char** argv) {
  auto start = system_clock::now();
  ALU4 c;
  CHECK(c.next());
  auto first = system_clock::now();
  std::cout << "first: " << duration_cast<microseconds>(first - start).count() << std::endl;
  for (int i = 0; i < 1000; ++i) {
    CHECK(c.next());
    // std::cout << i << ": " << c << std::endl;
  }
  auto end = system_clock::now();
  std::cout << "complete: " << duration_cast<milliseconds>(end - start).count() << std::endl;
  return 0;
}
