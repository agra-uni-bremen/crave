// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#include <string>

#include "../crave/ConstrainedRandom.hpp"

namespace crave {

std::string config_file_name = "crave.cfg";
std::string const& get_config_file_name() { return config_file_name; }
void set_config_file_name(std::string const& str) { config_file_name = str; }

void init() { init(get_config_file_name()); }

void init(std::string const& backend) { set_solver_backend(backend); }

int logging_severity = ERROR;

}  // namespace crave

// vim: tabstop=2 shiftwidth=2 expandtab
