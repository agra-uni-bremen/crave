#include <crave/ConstrainedRandom.hpp>
#include <iostream>

using std::ostream;

using crave::rand_obj;
using crave::randv;

class item : public rand_obj {
 public:
  item(rand_obj* parent = nullptr) : rand_obj(parent), src_addr(this), dest_addr(this) {
    auto dest_addrv = constraint.write_var(dest_addr);
    auto src_addrv = constraint.write_var(src_addr);
    constraint(src_addrv <= 20);
    constraint.soft(src_addrv % 4 == 0);
    constraint(dest_addrv <= 100);
  }

  friend ostream& operator<<(ostream& os, item& it) {
    os << it.src_addr << " " << it.dest_addr;
    return os;
  }

  randv<unsigned> src_addr;
  randv<unsigned> dest_addr;
};

class item_ext : public item {
 public:
  item_ext(rand_obj* parent = 0) : item(parent), last_dest_addr(0) {
    auto dest_addrv = constraint.write_var(dest_addr);
    auto src_addrv = constraint.write_var(src_addr);
    auto last_dest_addrv = constraint.read_var(last_dest_addr);
    // item_ext inherits all constraints of item
    constraint(src_addrv % 4 == 3);  // this constraint makes the soft constraint in item useless.
    constraint(dest_addrv > last_dest_addrv);
  }

  bool next() {
    // custom next() saves the generated value of dest_addr
    if (item::next()) {
      last_dest_addr = dest_addr;
      return true;
    }
    return false;
  }

  unsigned last_dest_addr;
};

int main(int argc, char* argv[]) {
  item it;
  for (int i = 0; i < 10; i++) {
    CHECK(it.next());
    std::cout << it << std::endl;
  }
  std::cout << "***********************" << std::endl;
  item_ext it_ext;
  while (it_ext.next()) {
    std::cout << it_ext << std::endl;
  }
  return 0;
}

#ifdef WITH_SYSTEMC
#include <systemc.h>
int sc_main(int argc, char* argv[]) { return main(argc, argv); }
#endif
