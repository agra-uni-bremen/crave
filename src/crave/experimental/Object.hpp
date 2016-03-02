// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <iostream>
#include <list>
#include <string>
#include <unordered_map>

namespace crave {

struct Generator;

struct crv_object_name;
class crv_object;

struct crv_object_name {
  crv_object_name(const char*);
  crv_object_name(const std::string& s) : crv_object_name(s.c_str()) {}
  crv_object_name(const crv_object_name&) = default;
  ~crv_object_name();

  std::string operator()() { return name_; }

  std::string name_;
  crv_object* object_;
};

class crv_object {
  template <typename T, typename Enable>
  friend class crv_variable;

 public:
  crv_object();
  ~crv_object();

  virtual std::string obj_kind() const { return "crv_object"; }
  virtual bool randomize() { return false; }

  std::string name() const { return name_; }
  std::string fullname() const { return fullname_; }
  void print_object_hierarchy(int level = 0) const;

  static crv_object* root();
  static crv_object* find(std::string);
  static unsigned count();

 protected:
  crv_object(const crv_object& other)
      : name_(other.name_), orig_name_(other.orig_name_), parent_(), children_(), fullname_(other.fullname_) {}

  void remove_child(crv_object*);

  virtual void request_rebuild() {
    if (parent_) parent_->request_rebuild();
  }

  void recursive_build(Generator& gen);

  std::string name_;
  std::string orig_name_;
  crv_object* parent_;
  std::list<crv_object*> children_;
  std::string fullname_;
  std::unordered_map<std::string, unsigned> local_name_map_;

 private:
  crv_object(int) : name_("root"), orig_name_(name_), parent_(), children_(), fullname_(name_) {}
};

};  // namespace crave
