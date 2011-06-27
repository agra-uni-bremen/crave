#include <ctime>

#include "ConstrainedRandom.hpp"

namespace platzhalter {

  int new_var_id() { static int _ID=0; return ++_ID;}

  //boost::mt19937 rng(std::time(0));
  boost::mt19937 rng(0);

  RandVecMap __rand_vec_map;

} // namespace platzhalter

// vim: tabstop=2 shiftwidth=2 expandtab
