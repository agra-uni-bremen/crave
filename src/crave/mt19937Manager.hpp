// Copyright 2014 The CRAVE developers. All rights reserved.//
#pragma once
#include <map>
#include <boost/random.hpp>

class mt19937Manager {
public:
    mt19937Manager(unsigned int seed);
    virtual ~mt19937Manager();
    void set_global_seed(unsigned int s);
    boost::mt19937* get();
private:
    void actualizeSeed();
    std::map<unsigned int, boost::mt19937*> randomMap_;
    unsigned int seed_;
};
