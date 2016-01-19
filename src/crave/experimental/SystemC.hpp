// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Variable.hpp"

#include "../frontend/bitsize_traits_sysc.hpp"
#include "../frontend/Distribution.hpp"

namespace crave {

template <typename T>
struct to_constant_expr<T, typename std::enable_if<is_sysc_dt<T>::value>::type> {
  Constant operator()(T value) { 
    return Constant(value.to_uint64(), bitsize_traits<T>::value, crave::is_signed<T>::value);
  }
};

template <typename T>
class crv_variable<T, typename std::enable_if<is_sysc_dt<T>::value>::type> : public crv_variable_base<T> {
  CRV_VARIABLE_COMMON_CONSTRUCTORS(T);
  CRV_VARIABLE_ASSIGNMENT_INTERFACE(T);                      
  CRV_VARIABLE_ARITHMETIC_INTERFACE(T);                      
  CRV_VARIABLE_BITWISE_INTERFACE(T);

 public:                                  
  bool randomize() override {             
    static distribution<int64_t> dist;    
    this->value = dist.nextValue();       
    return true;                          
  }
};

}  // namespace crave

