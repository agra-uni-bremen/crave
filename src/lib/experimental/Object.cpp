// Copyright 2014 The CRAVE developers. All rights reserved.

#include "../../crave/experimental/Object.hpp"

#include <cassert>
#include <stack>
#include <list>
#include <string>
#include <unordered_map>

namespace crave {

static std::stack<crv_object_name*> crv_name_stack;
static std::stack<crv_object*> crv_obj_stack;
static std::unordered_map<std::string, crv_object*> crv_obj_map;

crv_object_name::crv_object_name(const char* c) : name_(c), object_(0) {
  crv_name_stack.push(this);
}

crv_object_name::~crv_object_name() {
  assert(crv_obj_stack.top() == object_);
  crv_obj_stack.pop();
  object_ = 0;
}

crv_object_name::operator const char*() const { return name_; }

crv_object::crv_object() {
  assert(!crv_name_stack.empty());
  crv_name_stack.top()->object_ = this;
  name_ = *crv_name_stack.top();
  parent_ = !crv_obj_stack.empty() ? crv_obj_stack.top() : root();
  parent_->children_.push_back(this);
  fullname_ = (std::string(parent_->fullname_) + "." + name_).c_str();
  crv_obj_map[fullname_] = this;
  crv_obj_stack.push(this);
  crv_name_stack.pop();
}

crv_object::crv_object(const char* name) : name_(name), parent_(0), children_(), fullname_(name) { }

crv_object::~crv_object() {
  if (parent_)
    parent_->children_.remove(this);
  if (fullname_)
    crv_obj_map.erase(fullname_);
  fullname_ = 0;  
  name_ = 0;
  parent_ = 0;
}

void crv_object::print_object_hierarchy(int lvl) {
  std::cout << std::string(2 * lvl, ' ');
  std::cout << name() << " # " << fullname() << std::endl;
  for (crv_object* obj : children_)
    obj->print_object_hierarchy(lvl + 1);
}

crv_object* crv_object::root() {
  static crv_object root("root");
  return &root;    
}

unsigned crv_object::count() {
  return crv_obj_map.size();
}

crv_object* crv_object::find(const char* name) {
  auto tmp = crv_obj_map.find(name);
  return tmp != crv_obj_map.end() ? tmp->second : nullptr;    
}


}; // namespace crave

