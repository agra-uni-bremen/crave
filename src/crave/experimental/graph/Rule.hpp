// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.
#pragma once

#include "../../ConstrainedRandom.hpp"

namespace crave {

namespace graph {

struct Rule;
typedef std::map<std::string, Rule*> rule_map;
typedef std::function<void()> action_type;
extern rule_map global_rule_map;

/**
 * Graph-based Rule, highly experimental
 */
struct Rule {
  action_type entry; /**< TODO*/
  action_type main;  /**< TODO*/
  action_type exit;  /**< TODO*/

  /**
   * constructor
   * @param name name of this rule
   */
  Rule(const char* name);

  /**
   * get Rule name
   * @return name of the Rule
   */
  const char* name() const;

  /**
   * bind this Rule with and rand_obj
   */
  void bind_rand_obj(rand_obj* obj);

  /**
   * checks if the bound rand_obj is covered
   */
  bool is_rand_obj_covered() const;

  /**
   * reset coverate of the Rule
   */
  void reset_coverage();

 private:
  const char* m_name;
  rand_obj* m_rand_obj;

  void gen();
};
}
}
