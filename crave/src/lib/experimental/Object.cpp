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


#include "../../crave/experimental/Object.hpp"

#include <cassert>
#include <list>
#include <string>
#include <unordered_map>

namespace crave {

static std::list<crv_object_name*> crv_name_stack;
static std::list<crv_object*> crv_obj_stack;
static std::unordered_map<std::string, crv_object*> crv_obj_map;

crv_object_name::crv_object_name(const char* c) : name_(c), object_(0) { crv_name_stack.push_back(this); }

crv_object_name::~crv_object_name() {
  if (object_ == nullptr) return;
  assert(!crv_obj_stack.empty());
  assert(crv_obj_stack.back() == object_);
  crv_obj_stack.pop_back();
  object_ = nullptr;
}

crv_object::crv_object() {
  assert(!crv_name_stack.empty());
  crv_object_name* top = crv_name_stack.back();
  top->object_ = this;
  parent_ = !crv_obj_stack.empty() ? crv_obj_stack.back() : root();
  parent_->children_.push_back(this);  
  name_ = top->name_;
  orig_name_ = name_;
  // check name conflict
  auto& name_map = parent_->local_name_map_;
  if (name_map.find(name_) != name_map.end()) {
    name_ += std::to_string(name_map[name_]++);
  } else {
    name_map[name_] = 1;
  }
  // calculate fullname
  for (auto obj : crv_obj_stack) fullname_ += obj->name_ + ".";
  fullname_ += name_;
  crv_obj_map[fullname_] = this;
  crv_obj_stack.push_back(this);
  crv_name_stack.pop_back();
}

crv_object::~crv_object() {
  if (parent_) {
    parent_->remove_child(this);
    parent_ = nullptr;
  }
  crv_obj_map.erase(fullname_);
}

void crv_object::print_object_hierarchy(int lvl) const {
  std::cout << std::string(2 * lvl, ' ');
  std::cout << name() << " # " << fullname() << " @ " << this << ", parent @ " << parent_ << std::endl;
  for (auto obj : children_) obj->print_object_hierarchy(lvl + 1);
}

void crv_object::remove_child(crv_object* child) {
  children_.remove(child);
  if (--local_name_map_[child->orig_name_] == 0) local_name_map_.erase(child->name_);
}

crv_object* crv_object::root() {
  static crv_object root(0);
  return &root;
}

unsigned crv_object::count() { return crv_obj_map.size(); }

crv_object* crv_object::find(std::string name) {
  auto tmp = crv_obj_map.find(name);
  return tmp != crv_obj_map.end() ? tmp->second : nullptr;
}

};  // namespace crave
