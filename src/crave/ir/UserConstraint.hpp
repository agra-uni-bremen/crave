// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
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

struct UserConstraint;

struct UserVectorConstraint;
typedef boost::shared_ptr<UserVectorConstraint> VectorConstraintPtr;

}  // end namespace crave
