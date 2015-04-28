// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <iostream>
#include <stack>
#include <list>

namespace crave {

struct crv_object_name;
class crv_object;

struct crv_object_name {
  crv_object_name(const char*);
  ~crv_object_name();
  operator const char*() const;

  const char* name_;
  crv_object* object_;
};

class crv_object {
 public:
  crv_object();
  ~crv_object();

  virtual const char* kind() { return "crv_object"; }
  const char* name() { return name_; }
  const char* fullname() { return fullname_; } 
  void print_object_hierarchy(int level = 0);

  static crv_object* root();
  static crv_object* find(const char*);
  static unsigned count();

 protected:
  const char* name_;
  crv_object* parent_;
  std::list<crv_object*> children_;
  const char* fullname_;
  
 private:
  crv_object(const char* name);
};

};  // namespace crave
