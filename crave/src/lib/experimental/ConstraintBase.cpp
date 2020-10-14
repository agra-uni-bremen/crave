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

#include "../../crave/experimental/ConstraintBase.hpp"
#include "../../crave/backend/Generator.hpp"

namespace crave {
  crv_constraint_base::crv_constraint_base() : active_(true) {}
    
  std::string crv_constraint_base::obj_kind() const { return "crv_constraint"; }

  void crv_constraint_base::activate() {
    if (!active_) {
      active_ = true;
      request_rebuild();
    }
  }

  void crv_constraint_base::deactivate() {
    if (active_) {
      active_ = false;
      request_rebuild();
    }
  }

  bool crv_constraint_base::active() const { return active_; }

  void crv_constraint_base::recursive_build(Generator &gen) const {
    if (!active()) return;
    unsigned cnt = 0;
    if (!soft()) {
      for (auto e : expr_list()) gen(fullname() + "#" + std::to_string(cnt++), e);
    } else {
      for (auto e : expr_list()) gen.soft(fullname() + "#" + std::to_string(cnt++), e);
    }
  }
}
