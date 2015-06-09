// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <vector>
#include <set>

#include "Context.hpp"
#include "ConstraintManager.hpp"

namespace crave {

typedef boost::shared_ptr<UserVectorConstraint> VectorConstraintPtr;

struct ConstraintPartitioner {
  void reset() {
    constr_mngs_.clear();
    constraints_.clear();
    partitions_.clear();
    vec_constraints_.clear();
  }

  void mergeConstraints(const ConstraintManager& mng) {
    LOG(INFO) << "Merge set " << mng.id_ << " with set(s)";
    BOOST_FOREACH(unsigned id, constr_mngs_) { LOG(INFO) << " " << id; }
    constr_mngs_.insert(mng.id_);
    BOOST_FOREACH(ConstraintPtr c, mng.constraints_) {
      if (c->isEnabled()) {
        if (c->isVectorConstraint())
          vec_constraints_.push_back(
              boost::static_pointer_cast<UserVectorConstraint>(c));

        else
          constraints_.push_back(c);
      }
    }
  }

  void partition() {
    while (!constraints_.empty()) {
      ConstraintPartition cp;
      maximizePartition(&cp, &constraints_);
      partitions_.push_back(cp);
    }
    LOG(INFO) << "Partition results of set(s)";
    BOOST_FOREACH(unsigned id, constr_mngs_) { LOG(INFO) << " " << id; }
    LOG(INFO) << ": ";

    LOG(INFO) << "  " << vec_constraints_.size() << " vector constraint(s):";
    BOOST_FOREACH(VectorConstraintPtr c, vec_constraints_) {
      LOG(INFO) << "   " << c->name();
    }

    LOG(INFO) << "  " << partitions_.size() << " constraint partition(s):";
    uint cnt = 0;
    BOOST_FOREACH(ConstraintPartition& cp, partitions_) {
      LOG(INFO) << "    #" << ++cnt << ": " << cp;
    }
  }

  std::vector<ConstraintPartition>& getPartitions() { return partitions_; }

  std::vector<VectorConstraintPtr>& getVectorConstraints() {
    return vec_constraints_;
  }

 private:
  void maximizePartition(ConstraintPartition *cp, ConstraintList *lc) {
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
        std::set_intersection(cp->support_vars_.begin(),
                              cp->support_vars_.end(),
                              c->support_vars_.begin(), c->support_vars_.end(),
                              std::back_inserter(v_intersection));
        if (!v_intersection.empty()) {
          changed = true;
          cp->add(c);
          cp->support_vars_.insert(c->support_vars_.begin(),
                                  c->support_vars_.end());
          ite = lc->erase(ite);
        } else {
          ++ite;
        }
      }
      if (!changed) break;
    }
  }

 private:
  std::set<unsigned> constr_mngs_;
  ConstraintList constraints_;

  // results
  std::vector<ConstraintPartition> partitions_;
  std::vector<VectorConstraintPtr> vec_constraints_;
};


}  // namespace crave
