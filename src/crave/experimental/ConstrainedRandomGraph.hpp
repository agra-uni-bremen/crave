#pragma once

#include <iostream>
#include <string>
#include <set>
#include <functional>

#include <boost/shared_ptr.hpp>

#include "graph/Node.hpp"
#include "graph/Rule.hpp"
#include "graph/RuleContext.hpp"


#define NAMED_RULE(name) crave::graph::rule_type name = {crave::graph::Rule(#name)};
