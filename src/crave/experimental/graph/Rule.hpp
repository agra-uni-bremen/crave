// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.
#pragma once

#include "../../ConstrainedRandom.hpp"

namespace crave {

namespace graph {
    
struct Rule;
typedef std::map<std::string, Rule*> rule_map;
typedef std::function<void()> action_type;
extern rule_map global_rule_map;

struct Rule {
  action_type entry;
  action_type main;
  action_type exit;

  Rule(const char* name);

  const char* name() const;

  void bind_rand_obj(rand_obj* obj);

  bool is_rand_obj_covered() const;
  void reset_coverage();

 private:
  const char* m_name;
  rand_obj* m_rand_obj;

  void gen();
};
}
}
