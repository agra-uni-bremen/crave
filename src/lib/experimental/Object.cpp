// Copyright 2014 The CRAVE developers. All rights reserved.

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

void crv_object::print_object_hierarchy(int lvl) {
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
