#pragma once

#include "graph/Node.hpp"
#include "graph/Rule.hpp"

#define NAMED_RULE(name) crave::graph::rule_type name = {crave::graph::Rule(#name)};
