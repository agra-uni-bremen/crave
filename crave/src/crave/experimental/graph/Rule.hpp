/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/

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
