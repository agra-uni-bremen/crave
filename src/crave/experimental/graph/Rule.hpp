// Copyright 2014 The CRAVE developers. All rights reserved.
#pragma once

#include "../../ConstrainedRandom.hpp"

#include <boost/assert.hpp>
#include <boost/bind.hpp>

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

  const char* name() const { return m_name; }

  void bind_rand_obj(rand_obj* obj) {
    m_rand_obj = obj;
    main = boost::bind(&Rule::gen, this);
  }

  bool is_rand_obj_covered() const { return m_rand_obj == 0 || m_rand_obj->constraint.isCovered(); }
  void reset_coverage() {
    if (m_rand_obj) m_rand_obj->constraint.resetCoverage();
  }

 private:
  const char* m_name;
  rand_obj* m_rand_obj;

  void gen() {
    if (m_rand_obj) {
      if (is_rand_obj_covered())
        LOG(INFO) << "Rule " << m_name << " has its rand_obj covered, now generate a random solution";
      assert(m_rand_obj->next_cov());
    }
  }
};
}
}