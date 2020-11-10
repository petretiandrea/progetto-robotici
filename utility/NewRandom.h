//
// Created by Andrea Petreti on 05/11/2020.
//

#ifndef SWARM_GEN_NEWRANDOM_H
#define SWARM_GEN_NEWRANDOM_H

#include <random>
#include <map>
#include <chrono>
#include <algorithm>

namespace newrandom {

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
}
#endif //SWARM_GEN_NEWRANDOM_H


