#include <ctime>

#include "ConstrainedRandom.hpp"

namespace platzhalter {

  int new_var_id() { static int _ID=0; return ++_ID;}

  boost::mt19937 rng(std::time(0));

  Context get_context(std::string const & solvername) {
    return metaSMT_Context(solvername);
  }

} // namespace platzhalter

