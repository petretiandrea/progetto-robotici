//
// Created by Andrea Petreti on 05/11/2020.
//

#ifndef SWARM_GEN_UTILITY_H
#define SWARM_GEN_UTILITY_H

#include <vector>
#include <algorithm>

using namespace std;

template <class T> std::ostream& operator<<(std::ostream &flux, const vector<T>& v) {
    for(auto elem : v) { flux << elem << " "; }
    return flux;
}

template<typename T> vector<T> fillVector(T elem, int size) {
    vector<T> v(size);
    fill(v.begin(), v.end(), elem);
    return v;
}

template<typename T> vector<T> fillVectorByFunction(int size, function<T(int)> map) {
    vector<T> v(size);
    for(int i = 0; i < size; i++) v[i] = map(i);
    return v;
}

int boolToInt(bool value) { return value ? 1 : 0; }

#endif //SWARM_GEN_UTILITY_H
