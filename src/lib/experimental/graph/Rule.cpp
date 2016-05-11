// Copyright 2014 The CRAVE developers. All rights reserved.
#include "../../../crave/experimental/graph/Rule.hpp"

namespace crave {

namespace graph {
    
    Rule::Rule(const char* name): entry(), main(), exit(), m_name(name), m_rand_obj(0) { global_rule_map[name] = this; }
    
    const char* Rule::name() const { return m_name; }
    
    void Rule::bind_rand_obj(rand_obj* obj) {
    m_rand_obj = obj;
    main = boost::bind(&Rule::gen, this);
  }
    bool Rule::is_rand_obj_covered() const { return m_rand_obj == 0 || m_rand_obj->constraint.isCovered(); }
    
    void Rule::reset_coverage() {
    if (m_rand_obj) m_rand_obj->constraint.resetCoverage();
  }
    void Rule::gen() {
    if (m_rand_obj) {
      if (is_rand_obj_covered())
        LOG(INFO) << "Rule " << m_name << " has its rand_obj covered, now generate a random solution";
      assert(m_rand_obj->next_cov());
    }
  }
}
}