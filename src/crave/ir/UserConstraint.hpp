// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <list>
#include <map>
#include <ostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "ConstraintManager.hpp"
#include "ConstraintPartition.hpp"
#include "ConstraintPartitioner.hpp"
#include "Context.hpp"
#include "Node.hpp"
#include "UserConstraintType.hpp"
#include "UserVectorConstraint.hpp"
#include "visitor/FixWidthVisitor.hpp"
#include "visitor/GetSupportSetVisitor.hpp"
#include "visitor/ToDotNodeVisitor.hpp"

namespace crave {

int new_constraint_id();

struct ConstraintManager;
struct ConstraintPartitioner;

class UserConstraint;

class UserVectorConstraint;
typedef std::shared_ptr<UserVectorConstraint> VectorConstraintPtr;

}  // end namespace crave
