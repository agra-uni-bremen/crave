// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <vector>
#include <string>

#include "Object.hpp"
#include "Variable.hpp"

namespace crave {

template <typename T, unsigned N>
class crv_array : public crv_object {
 public:
  crv_array(crv_object_name) : arr_() {
    for (unsigned i = 0; i < N; i++) {
      std::string s = std::string("element[") + std::to_string(i) + "]";
      arr_.emplace_back(new crv_variable<T>{s.c_str()});
    }
  }

  std::string kind() override { return "crv_array"; }

  expression sum() {
    expression result = value_to_expression(0);
    for (crv_variable<T>* v : arr_) result = crave::make_expression(result + (*v)());
    return result;
  }

  crv_variable<T>& operator[](unsigned pos) { return *arr_[pos]; }

 private:
  std::vector<crv_variable<T>*> arr_;
};

}  // namespace crave
