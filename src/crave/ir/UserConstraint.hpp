// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <glog/logging.h>

#include <ostream>
#include <string>
#include <vector>
#include <list>
#include <stdexcept>
#include <map>
#include <set>

#include "UserConstraintType.hpp"
#include "UserVectorConstraint.hpp"
#include "ConstraintPartition.hpp"
#include "ConstraintManager.hpp"
#include "ConstraintPartitioner.hpp"

#include "Context.hpp"
#include "Node.hpp"
#include "visitor/FixWidthVisitor.hpp"
#include "visitor/GetSupportSetVisitor.hpp"
#include "visitor/ToDotNodeVisitor.hpp"

namespace crave {

int new_constraint_id();

struct ConstraintManager;
struct ConstraintPartitioner;

class UserConstraint;

class UserVectorConstraint;
typedef boost::shared_ptr<UserVectorConstraint> VectorConstraintPtr;

}  // end namespace crave
