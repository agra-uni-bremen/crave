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

#include "../crave/ir/ConstraintManager.hpp"
#include "../crave/ir/visitor/GetSupportSetVisitor.hpp"
#include "../crave/ir/visitor/ToDotNodeVisitor.hpp"
#include "../crave/ir/visitor/ComplexityEstimationVisitor.hpp"
#include "../crave/utils/Logging.hpp"

namespace crave {

template <typename ostream>
ostream& operator<<(ostream& os, const ConstraintManager& set) {
  os << "Set " << set.id_ << " has " << set.constraints_.size() << " constraint(s) and has "
     << (set.changed_ ? "" : "not ") << "changed" << std::endl;
  for(ConstraintPtr item : set.constraints_) { os << item << std::endl; }
  os << std::flush;
  return os;
}
template std::ostream& operator<<<std::ostream>(std::ostream& os, const ConstraintManager& set);

ConstraintManager::ConstraintManager() : changed_(false) {
  static unsigned ID = 0;
  id_ = ++ID;
}

bool ConstraintManager::enableConstraint(std::string const& key) {
  LOG(INFO) << "Enable constraint " << key << " in set " << id_ << ": ";
  ConstraintMap::iterator ite = constr_map_.find(key);
  if (ite != constr_map_.end()) {
    if (!ite->second->isEnabled()) {
      LOG(INFO) << "  ok";
      ite->second->enable();
      changed_ = true;
    } else {
      LOG(INFO) << "  already enabled";
    }
    return true;
  }
  LOG(INFO) << "not found";
  return false;
}

bool ConstraintManager::disableConstraint(std::string const& key) {
  LOG(INFO) << "Disable constraint " << key << " in set " << id_ << ": ";
  ConstraintMap::iterator ite = constr_map_.find(key);
  if (ite != constr_map_.end()) {
    if (ite->second->isEnabled()) {
      LOG(INFO) << "  ok";
      ite->second->disable();
      changed_ = true;
    } else {
      LOG(INFO) << "  already enabled";
    }
    return true;
  }
  LOG(INFO) << "  not found";
  return false;
}

bool ConstraintManager::isConstraintEnabled(std::string const& key) {
  ConstraintMap::iterator ite = constr_map_.find(key);
  return ite != constr_map_.end() && ite->second->isEnabled();
}

bool ConstraintManager::isChanged() const { return changed_; }

void ConstraintManager::resetChanged() { changed_ = false; }

ConstraintPtr ConstraintManager::makeConstraint(std::string const& name, int c_id, NodePtr n, Context* ctx,
                                                bool const soft, bool const cover) {
  LOG(INFO) << "New " << (soft ? "soft " : "") << (cover ? "cover " : "") << "constraint " << name << " in set " << id_;

  if (constr_map_.find(name) != constr_map_.end()) {
    throw std::runtime_error(name + "Constraint already exists.");
  }

  GetSupportSetVisitor gssv;
  n->visit(&gssv);

  ConstraintPtr c;

  if (boost::dynamic_pointer_cast<ForEach>(n) != 0) {
    c = std::make_shared<UserVectorConstraint>(c_id, n, name, gssv.getSupportVars(), false, soft, cover);
  } else if (boost::dynamic_pointer_cast<Unique>(n) != 0) {
    c = std::make_shared<UserVectorConstraint>(c_id, n, name, gssv.getSupportVars(), true, soft, cover);
  } else {
    c = std::make_shared<UserConstraint>(c_id, n, name, gssv.getSupportVars(), soft, cover);
  }

  assert(!c->isSoft() || !c->isCover());              // soft cover constraint not defined/supported yet
  assert(!c->isVectorConstraint() || !c->isCover());  // cover vector constraint not defined/supported yet

  if (!c->isVectorConstraint()) {
    ComplexityEstimationVisitor cev;
    c->complexity_ = cev.getComplexityEstimation(*n);
  }

  changed_ = true;
  constraints_.push_back(c);

  return constr_map_[name] = c;
}

std::ostream& ConstraintManager::printDotGraph(std::ostream& os) {
  ToDotVisitor visitor(os);
  for(ConstraintPtr c : constraints_) {
    size_t a = reinterpret_cast<size_t>(&*c);
    size_t b = reinterpret_cast<size_t>(&(*c->expr()));
    os << "\t" << a << " [label=\"" << c->name() << (c->isSoft() ? " soft" : "") << (c->isCover() ? " cover" : "")
       << (!c->isEnabled() ? " disabled" : "") << "\"]" << std::endl;
    os << "\t" << a << " -> " << b << std::endl;
    c->expr()->visit(&visitor);
  }
  return os;
}
}
