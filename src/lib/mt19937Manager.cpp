// Copyright 2014 The CRAVE developers. All rights reserved.//
#include "../crave/mt19937Manager.hpp"
#include <ctime>

mt19937Manager::mt19937Manager(unsigned int seed) : seed_(seed) {
}

mt19937Manager::~mt19937Manager() {
    for (auto tuple : randomMap_) {
        delete tuple.second;
    }
}

void mt19937Manager::set_global_seed(unsigned int s) {
    seed_ = s;
    actualizeSeed();
}

boost::mt19937* mt19937Manager::get() {
        unsigned int processId = 0;
        if (randomMap_.count(processId)) { //constains
            return randomMap_.at(processId);
        }
        boost::mt19937* rnd = new boost::mt19937(seed_);
        randomMap_.insert(std::make_pair(processId, rnd));
        return rnd;
    }

void mt19937Manager::actualizeSeed() {
        for (auto tuple : randomMap_) {
            tuple.second->seed(seed_);
        }
    }

