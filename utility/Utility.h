//
// Created by Andrea Petreti on 05/11/2020.
//
# pragma once

#ifndef SWARM_GEN_UTILITY_H
#define SWARM_GEN_UTILITY_H

#include <vector>
#include "NewRandom.h"

using namespace std;

class utility {

public:
    template<typename T> static std::vector<T> extractFromCollection(newrandom::Random& rnd, std::vector<T> collection, int numberOfElements, bool allowRepetition = false);
    static std::vector<int> extractFromRange(newrandom::Random& rnd, int min, int max, int numberOfElements, bool allowRepetition = false);
    static int boolToInt(bool value, bool invert = false);
    static string createMetaInfoFilename();
};

template <class T> std::ostream& operator<<(std::ostream &flux, const vector<T>& v) {
    for(auto elem : v) { flux << elem << " "; }
    return flux;
}

#endif