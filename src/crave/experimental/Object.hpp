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
  ~crv_object_name();
  operator std::string() const { return name_; }

  std::string name_;
  crv_object* object_;
};

class crv_object {
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
      : name_(other.name_), parent_(other.parent_), children_(other.children_), fullname_(other.fullname_) {}

  std::string name_;
  crv_object* parent_;
  std::list<crv_object*> children_;
  std::string fullname_;
  std::unordered_map<std::string, unsigned> local_name_map_;

 private:
  crv_object(std::string name);
};

};  // namespace crave
