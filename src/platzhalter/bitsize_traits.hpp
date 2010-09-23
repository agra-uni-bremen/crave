#pragma once

namespace platzhalter {

  template <typename Integer>
  struct bitsize_traits {
    static const unsigned nbits = sizeof(Integer)*8;
  };

  template<>
  struct bitsize_traits<bool> {
    static const unsigned nbits = 1;
  };

  template<typename T> struct randv;
  
  template<typename T>
  struct bitsize_traits< randv<T> > : public bitsize_traits<T> {};

} // namespace platzhalter

//  vim: ft=cpp:ts=2:sw=2:expandtab
