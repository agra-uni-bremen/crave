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

#include "../crave/ir/UserConstraintType.hpp"

namespace crave {

UserConstraint::UserConstraint(unsigned const id, UserConstraint::expression const expr, std::string const& name,
                               std::set<int> support_vars, bool const soft, bool const cover, bool const enabled)
    : id_(id),
      expr_(expr),
      name_(name),
      support_vars_(support_vars),
      soft_(soft),
      cover_(cover),
      enabled_(enabled),
      complexity_(0) {}

template <typename ostream>
ostream& operator<<(ostream& os, const UserConstraint& constr) {
  os << constr.name_ << " is a " << (constr.soft_ ? "soft" : "hard") << " constraint and "
     << (constr.enabled_ ? "enabled" : "disabled");
  os << ", support vars =";

  for(int item : constr.support_vars_) { os << " " << item; }
  return os;
}

template std::ostream& operator<<<std::ostream>(std::ostream& os, const UserConstraint& constr);

unsigned UserConstraint::id() const { return id_; }

UserConstraint::expression const& UserConstraint::expr() const { return expr_; }

std::string UserConstraint::name() const { return name_; }

bool UserConstraint::isSoft() const { return soft_; }

bool UserConstraint::isCover() const { return cover_; }

bool UserConstraint::isEnabled() const { return enabled_; }

void UserConstraint::enable() { enabled_ = true; }

void UserConstraint::disable() { enabled_ = false; }

bool UserConstraint::isVectorConstraint() { return false; }

unsigned UserConstraint::complexity() const { return complexity_; }
}
