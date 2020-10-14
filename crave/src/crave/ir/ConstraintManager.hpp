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


#pragma once

#include <string>
#include <map>

#include "UserConstraintType.hpp"
#include "UserVectorConstraint.hpp"
#include "Context.hpp"
#include "visitor/FixWidthVisitor.hpp"

namespace crave {

int new_constraint_id();

struct ConstraintManager {
  friend struct ConstraintPartitioner;

  typedef std::map<std::string, ConstraintPtr> ConstraintMap;

  ConstraintManager();

  bool enableConstraint(std::string const& key);

  bool disableConstraint(std::string const& key);

  bool isConstraintEnabled(std::string const& key);

  bool isChanged() const;

  void resetChanged();

  template <typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, int c_id, Expr e, Context* ctx, bool const soft = false,
                               bool const cover = false) {
    FixWidthVisitor fwv;
    NodePtr n(fwv.fixWidth(*boost::proto::eval(e, *ctx)));
    return ConstraintManager::makeConstraint(name, c_id, n, ctx, soft, cover);
  }

  ConstraintPtr makeConstraint(std::string const& name, int c_id, NodePtr n, Context* ctx, bool const soft = false,
                               bool const cover = false);

  template <typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, Expr e, Context* ctx, bool const soft = false,
                               bool const cover = false) {
    return makeConstraint(name, new_constraint_id(), e, ctx, soft, cover);
  }

  template <typename Expr>
  ConstraintPtr makeConstraint(Expr e, Context* ctx, bool const soft = false, bool const cover = false) {
    int id = new_constraint_id();
    return makeConstraint("constraint_" + std::to_string(id), id, e, ctx, soft, cover);
  }

  std::ostream& printDotGraph(std::ostream& os);

  template <typename ostream>
  friend ostream& operator<<(ostream& os, const ConstraintManager& set);

 private:
  unsigned id_;
  ConstraintMap constr_map_;
  ConstraintList constraints_;
  bool changed_;
};

template <typename ostream>
ostream& operator<<(ostream& os, const ConstraintManager& set);
}  // namespace crave
