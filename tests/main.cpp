#include <crave/ConstrainedRandom.hpp>
#undef CHECK

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

int main(int argc, char** argv) {
  for (int i = 0; i < argc; i++) {
    static const char prefix[] = "--backend-";
    if (!std::strncmp(argv[i], prefix, sizeof(prefix) - 1)) {
      crave::set_solver_backend(argv[i] + sizeof(prefix) - 1);
    }
  }
  doctest::Context context;
  context.applyCommandLine(argc, argv);
  return context.run();
}

#ifdef WITH_SYSTEMC
#include <systemc.h>
int sc_main(int argc, char* argv[]) { return main(argc, argv); }
#endif
