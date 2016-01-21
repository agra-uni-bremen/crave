// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <iostream>
#include <list>
#include <string>
#include <unordered_map>

namespace crave {

struct crv_object_name;
class crv_object;

struct crv_object_name {
  crv_object_name(const char*);
  crv_object_name(const crv_object_name&) = default;
  ~crv_object_name();

  std::string name_;
  crv_object* object_;
};

class crv_object {
  template <typename T, typename Enable>
  friend class crv_variable;

 public:
  crv_object();
  ~crv_object();

  virtual std::string kind() { return "crv_object"; }
  virtual bool randomize() { return false; }

  std::string name() { return name_; }
  std::string fullname() { return fullname_; }
  void print_object_hierarchy(int level = 0);

  static crv_object* root();
  static crv_object* find(std::string);
  static unsigned count();

 protected:
  crv_object(const crv_object& other)
      : name_(other.name_), orig_name_(other.orig_name_), parent_(other.parent_), children_(other.children_), fullname_(other.fullname_) {}

  void remove_child(crv_object*);

  virtual void request_rebuild() {
    if (parent_ != nullptr) {
      parent_->request_rebuild();
    }
  }

  std::string name_;
  std::string orig_name_;
  crv_object* parent_;
  std::list<crv_object*> children_;
  std::string fullname_;
  std::unordered_map<std::string, unsigned> local_name_map_;

 private:
  crv_object(int) : name_("root"), orig_name_(name_), parent_(0), children_(), fullname_(name_) {}
};

};  // namespace crave
