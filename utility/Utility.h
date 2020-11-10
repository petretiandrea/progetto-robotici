//
// Created by Andrea Petreti on 05/11/2020.
//
# pragma once

#ifndef SWARM_GEN_UTILITY_H
#define SWARM_GEN_UTILITY_H

#include <vector>
#include <algorithm>
#include "NewRandom.h"

using namespace std;

class utility {

public:
    template<typename T> static std::vector<T> extractFromCollection(newrandom::Random& rnd, std::vector<T> collection, int numberOfElements, bool allowRepetition = false) {
        std::vector<T> result;
        std::vector<T> toConsider(collection);

        for(int i = 0; i < numberOfElements; i++) {
            if (allowRepetition) {
                result.push_back(toConsider[rnd.nextInt(0, toConsider.size() - 1)]);
            } else {
                auto indexElem = rnd.nextInt(0, toConsider.size() - 1);
                result.push_back(toConsider[indexElem]);
                toConsider.erase(toConsider.begin() + indexElem);
            }
        }
        return result;
    }

    static std::vector<int> extractFromRange(newrandom::Random& rnd, int min, int max, int numberOfElements, bool allowRepetition = false) {
        std::vector<int> collection(max - min + 1);
        std::iota(collection.begin(), collection.end(), min);
        return extractFromCollection(rnd, collection, numberOfElements, allowRepetition);
    }

    static int boolToInt(bool value, bool invert = false) { return invert ? (!value ? 1 : 0) : (value ? 1 : 0); }

    template<typename T> static vector<T> fillVector(T elem, int size) {
        vector<T> v(size);
        fill(v.begin(), v.end(), elem);
        return v;
    }

    template<typename T> static vector<T> fillVectorByFunction(int size, function<T(int)> map) {
        vector<T> v(size);
        for(int i = 0; i < size; i++) v[i] = map(i);
        return v;
    }

};

template <class T> std::ostream& operator<<(std::ostream &flux, const vector<T>& v) {
    for(auto elem : v) { flux << elem << " "; }
    return flux;
}

#endif