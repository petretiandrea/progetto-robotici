//
// Created by Andrea Petreti on 05/11/2020.
//
# pragma once

#ifndef SWARM_GEN_UTILITY_H
#define SWARM_GEN_UTILITY_H

#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <ga/GAEvalData.h>
#include "NewRandom.h"
#include "Printable.h"

using namespace std;

class utility {

public:
    template<typename T> static std::vector<T> extractFromCollection(newrandom::Random& rnd, std::vector<T> collection, int numberOfElements, bool allowRepetition = false);
    static std::vector<int> extractFromRange(newrandom::Random& rnd, int min, int max, int numberOfElements, bool allowRepetition = false);
    static int boolToInt(bool value, bool invert = false);
    static string createMetaInfoFilename();

    static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                        std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
    }
};

template <class T> std::ostream& operator<<(std::ostream &flux, const vector<T>& v) {
    for(auto elem : v) { flux << elem << " "; }
    return flux;
}

ostream& operator <<(ostream& os, GAEvalData& data);
#endif