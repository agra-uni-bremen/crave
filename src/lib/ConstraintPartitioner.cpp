#include "../crave/ir/ConstraintPartitioner.hpp"

namespace crave {
    
    void ConstraintPartitioner::reset() {
    constr_mngs_.clear();
    constraints_.clear();
    partitions_.clear();
    vec_constraints_.clear();
  }

  void ConstraintPartitioner::mergeConstraints(const ConstraintManager& mng) {
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

  void ConstraintPartitioner::partition() {
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

  std::vector<ConstraintPartition>& ConstraintPartitioner::getPartitions() { return partitions_; }

  std::vector<VectorConstraintPtr>& ConstraintPartitioner::getVectorConstraints() {
    return vec_constraints_;
  }
  
  void ConstraintPartitioner::maximizePartition(ConstraintPartition *cp, ConstraintList *lc) {
    ConstraintPtr c = lc->front();
    lc->pop_front();
    cp->support_vars_ = c->support_vars_;
    cp->add(c);
    while (true) {
      bool changed = false;
      ConstraintList::iterator ite = lc->begin();

      while (ite != lc->end()) {
        c = *ite;
        if(c->support_vars_.size() == 1)
        {
            unsigned int varID = (*(c->support_vars_.begin()));
            if(cp->singleVariableConstraintMap_.count(varID))
            {
                ConstraintList& value = cp->singleVariableConstraintMap_.at(varID);
                value.push_back(c);            
            }   
            else
            {
                ConstraintList tmp;
                tmp.push_back(c);
                cp->singleVariableConstraintMap_[varID] = tmp;
            }
        }
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
}
