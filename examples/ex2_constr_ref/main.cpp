#include <crave/ConstrainedRandom.hpp>
#include <crave/ir/ConstraintPartition.hpp>
#include <iostream>

using std::ostream;

using crave::rand_obj;
using crave::randv;
using crave::range;
using crave::weighted_range;

class item : public rand_obj {
 public:
  item(rand_obj* parent = nullptr) : rand_obj(parent), dest_addr(this) {
    auto dest_addrv = constraint.write_var(dest_addr);
    auto src_addrv = constraint.read_var(src_addr);
    constraint(dest_addrv % 4 == 0);
    constraint(dest_addrv >= src_addrv + 3);
  }

  friend ostream& operator<<(ostream& os, item& it) {
    os << it.src_addr << " " << it.dest_addr;
    return os;
  }

  unsigned src_addr;
  randv<unsigned> dest_addr;
};

int main(int argc, char* argv[]) {
  item it;
  for (unsigned i = 0; i < 20; i++) {
    it.src_addr = i;
    CHECK(it.next());
    std::cout << it << std::endl;
  }
  return 0;
}

#ifdef WITH_SYSTEMC
#include <systemc.h>
int sc_main(int argc, char* argv[]) { return main(argc, argv); }
#endif
