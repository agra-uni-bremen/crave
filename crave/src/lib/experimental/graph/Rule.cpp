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

#include "../../../crave/experimental/graph/Rule.hpp"
#include "../../../crave/utils/Logging.hpp"

#include <functional>

namespace crave {

namespace graph {
    
    Rule::Rule(const char* name): entry(), main(), exit(), m_name(name), m_rand_obj(0) { global_rule_map[name] = this; }
    
    const char* Rule::name() const { return m_name; }
    
    void Rule::bind_rand_obj(rand_obj* obj) {
    m_rand_obj = obj;
    main = std::bind(&Rule::gen, this);
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
