// Copyright 2014 The CRAVE developers. All rights reserved.//
#pragma once
#include <map>
#include <boost/random.hpp>

class mt19937Manager {
public:
    mt19937Manager(unsigned int seed);
    virtual ~mt19937Manager();

    void set_global_seed(unsigned int s) {
        seed_ = s;
        actualizeSeed();
    }

    boost::mt19937* get() {
        unsigned int processId = 0;
        if (randomMap_.count(processId)) { //constains
            return randomMap_.at(processId);
        }
        boost::mt19937* rnd = new boost::mt19937(seed_);
        randomMap_.insert(std::make_pair(processId, rnd));
        return rnd;
    }

private:

    void actualizeSeed() {
        for (auto tuple : randomMap_) {
            tuple.second->seed(seed_);
        }
    }
    std::map<unsigned int, boost::mt19937*> randomMap_;
    unsigned int seed_;


};
