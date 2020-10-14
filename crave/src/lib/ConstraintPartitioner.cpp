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

#include "../crave/ir/ConstraintPartitioner.hpp"
#include "../crave/utils/Logging.hpp"

#include <algorithm>

namespace crave {

void ConstraintPartitioner::reset() {
  constr_mngs_.clear();
  constraints_.clear();
  partitions_.clear();
  vec_constraints_.clear();
}

void ConstraintPartitioner::mergeConstraints(const ConstraintManager& mng) {
  LOG(INFO) << "Merge set " << mng.id_ << " with set(s)";
  for(unsigned id : constr_mngs_) { LOG(INFO) << " " << id; }
  constr_mngs_.insert(mng.id_);
  for(ConstraintPtr c : mng.constraints_) {
    if (c->isEnabled()) {
      if (c->isVectorConstraint())
        vec_constraints_.push_back(std::static_pointer_cast<UserVectorConstraint>(c));
      else
        constraints_.push_back(c);
    }
  }
}

void ConstraintPartitioner::partition() {
  while (!constraints_.empty()) {
    ConstraintPartition cp;
    maximizePartition(&cp, &constraints_);
    partitions_.push_back(cp);
  }
  LOG(INFO) << "Partition results of set(s)";
  for(unsigned id : constr_mngs_) { LOG(INFO) << " " << id; }
  LOG(INFO) << ": ";

  LOG(INFO) << "  " << vec_constraints_.size() << " vector constraint(s):";
  for(VectorConstraintPtr c : vec_constraints_) { LOG(INFO) << "   " << c->name(); }

  LOG(INFO) << "  " << partitions_.size() << " constraint partition(s):";
  unsigned cnt = 0;
  for(ConstraintPartition & cp : partitions_) { LOG(INFO) << "    #" << ++cnt << ": " << cp; }
}

std::vector<ConstraintPartition>& ConstraintPartitioner::getPartitions() { return partitions_; }

std::vector<VectorConstraintPtr>& ConstraintPartitioner::getVectorConstraints() { return vec_constraints_; }

void ConstraintPartitioner::maximizePartition(ConstraintPartition* cp, ConstraintList* lc) {
  ConstraintPtr c = lc->front();
  lc->pop_front();
  cp->support_vars_ = c->support_vars_;
  cp->add(c);
  while (true) {
    bool changed = false;
    ConstraintList::iterator ite = lc->begin();

    while (ite != lc->end()) {
      c = *ite;
      std::vector<int> v_intersection;
      std::set_intersection(cp->support_vars_.begin(), cp->support_vars_.end(), c->support_vars_.begin(),
                            c->support_vars_.end(), std::back_inserter(v_intersection));
      if (!v_intersection.empty()) {
        changed = true;
        cp->add(c);
        cp->support_vars_.insert(c->support_vars_.begin(), c->support_vars_.end());
        ite = lc->erase(ite);
      } else {
        ++ite;
      }
    }
    if (!changed) break;
  }
}
}
