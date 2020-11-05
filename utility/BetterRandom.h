//
// Created by Andrea Petreti on 05/11/2020.
//

#ifndef SWARM_GEN_BETTERRANDOM_H
#define SWARM_GEN_BETTERRANDOM_H

#include <random>
#include <map>
#include <chrono>
#include <algorithm>

namespace strongrandom {

    typedef std::mt19937 RNG;

    class Random {

        public:
            Random();
            explicit Random(unsigned int seed);

            double next();
            int nextInt(int min, int max);

        private:
            static std::uniform_real_distribution<double> distribution;
            RNG generator;

    };


    std::uniform_real_distribution<double> Random::distribution = std::uniform_real_distribution<double>(0.0, 1.0);

    Random::Random() : generator(std::mt19937(std::chrono::high_resolution_clock::now().time_since_epoch().count())) { }
    Random::Random(unsigned int seed) : generator(std::mt19937(seed)) { }

    double Random::next() {
        return distribution(generator);;
    }

    int Random::nextInt(int min, int max) {
        return (int) floor(next() * (max - min + 1)) + min;
    }


    template<typename T> std::vector<T> extractFromCollection(Random& rnd, std::vector<T> collection, int numberOfElements, bool allowRepetition = false) {
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

    std::vector<int> extractFromRange(Random& rnd, int min, int max, int numberOfElements, bool allowRepetition = false) {
        std::vector<int> collection(max - min + 1);
        std::iota(collection.begin(), collection.end(), min);
        return extractFromCollection(rnd, collection, numberOfElements, allowRepetition);
    }
}

#endif //SWARM_GEN_BETTERRANDOM_H
