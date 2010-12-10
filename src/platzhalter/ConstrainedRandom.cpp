#include <ctime>

#include "ConstrainedRandom.hpp"

namespace platzhalter {

  std::string default_solver() {return "SolverSWORD";}

  int new_var_id() { static int _ID=0; return ++_ID;}

  boost::mt19937 rng(std::time(0));

  Context get_context(std::string const & solvername) {
    if(solvername.empty()) {
      return metaSMT_Context(default_solver());
    } else {
      return metaSMT_Context(solvername);
    }
  }

} // namespace platzhalter

// vim: tabstop=2 shiftwidth=2 expandtab
