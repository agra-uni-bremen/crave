#include "../crave/mt19937Manager.hpp"
#include <ctime>

mt19937Manager::mt19937Manager(unsigned int seed) : seed_(seed) {
}

mt19937Manager::~mt19937Manager() {
    for (auto tuple : randomMap_) {
        delete tuple.second;
    }
}

