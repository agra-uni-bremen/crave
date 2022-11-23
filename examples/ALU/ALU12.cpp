#include <systemc.h>

#include <chrono>
#include <crave/ConstrainedRandom.hpp>
#include <crave/SystemC.hpp>

using crave::rand_obj;
using crave::randv;
using sc_dt::sc_bv;
using sc_dt::sc_uint;

using namespace std::chrono;

struct ALU12 : public rand_obj {
  randv<sc_bv<2> > op;
  randv<sc_uint<12> > a, b;

  ALU12(rand_obj* parent = 0) : rand_obj(parent), op(this), a(this), b(this) {
    auto opv = constraint.write_var(op);
    auto av = constraint.write_var(a);
    auto bv = constraint.write_var(b);
    // constraint(dist(opv, distribution<short>::simple_range(0, 3)));
    constraint((opv != 0x0) || (4095u >= av + bv));
    constraint((opv != 0x1) || ((4095u >= av - bv) && (bv <= av)));
    constraint((opv != 0x2) || (4095 >= av * bv));
    constraint((opv != 0x3) || (bv != 0));
  }

  friend std::ostream& operator<<(std::ostream& o, ALU12 const& alu) {
    o << alu.op << ' ' << alu.a << ' ' << alu.b;
    return o;
  }
};

int sc_main(int argc, char** argv) {
  auto start = system_clock::now();
  ALU12 c;
  CHECK(c.next());
  auto first = system_clock::now();
  std::cout << "first: " << duration_cast<microseconds>(first - start).count() << std::endl;
  for (int i = 0; i < 1000; ++i) {
    CHECK(c.next());
    std::cout << c << std::endl;
  }
  auto end = system_clock::now();
  std::cout << "complete: " << duration_cast<milliseconds>(end - start).count() << std::endl;
  return 0;
}