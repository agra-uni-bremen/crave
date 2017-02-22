// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <vector>
#include <set>

#include "Context.hpp"
#include "ConstraintManager.hpp"
#include "ConstraintPartition.hpp"

namespace crave {

typedef std::shared_ptr<UserVectorConstraint> VectorConstraintPtr;

struct ConstraintPartitioner {
  void reset();

  void mergeConstraints(const ConstraintManager& mng);

  void partition();

  std::vector<ConstraintPartition>& getPartitions();

  std::vector<VectorConstraintPtr>& getVectorConstraints();

 private:
  void maximizePartition(ConstraintPartition* cp, ConstraintList* lc);

 private:
  std::set<unsigned> constr_mngs_;
  ConstraintList constraints_;

  // results
  std::vector<ConstraintPartition> partitions_;
  std::vector<VectorConstraintPtr> vec_constraints_;
};

}  // namespace crave
