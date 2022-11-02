#include <crave/ConstrainedRandom.hpp>

using crave::Generator;
using crave::randv;

int main(int argc, char* argv[]) {
  randv<int> x(nullptr);
  randv<int> y(nullptr);
  Generator gen;
  auto xv = gen.write_var(x);
  auto yv = gen.write_var(y);
  gen(xv >= 0 && yv >= 0)(xv + yv == 4)(xv * yv == 4);

  CHECK(gen.next());
  std::cout << x << " " << y << std::endl;

  return 0;
}

#ifdef WITH_SYSTEMC
#include <systemc.h>
int sc_main(int argc, char* argv[]) { return main(argc, argv); }
#endif
