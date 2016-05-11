// Copyright 2014 The CRAVE developers. All rights reserved.
#include "../../../crave/experimental/graph/Rule.hpp"

namespace crave {

namespace graph {
    
    Rule::Rule(const char* name): entry(), main(), exit(), m_name(name), m_rand_obj(0) { global_rule_map[name] = this; }
}
}