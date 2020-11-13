//
// Created by Andrea Petreti on 13/11/2020.
//

#include "Utility.h"
#include <constants.h>
#include <algorithm>
#include <sstream>

template<typename T> std::vector<T> utility::extractFromCollection(newrandom::Random& rnd, std::vector<T> collection, int numberOfElements, bool allowRepetition) {
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

std::vector<int> utility::extractFromRange(newrandom::Random& rnd, int min, int max, int numberOfElements, bool allowRepetition) {
    std::vector<int> collection(max - min + 1);
    std::iota(collection.begin(), collection.end(), min);
    return extractFromCollection(rnd, collection, numberOfElements, allowRepetition);
}

int utility::boolToInt(bool value, bool invert) { return invert ? (!value ? 1 : 0) : (value ? 1 : 0); }

string utility::createMetaInfoFilename() {
    std::ostringstream metainfo;
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (buffer,80,"%m-%d_%H-%M",timeinfo);
    metainfo
            << string(buffer)
            << "_p" << constants::POPULATION
            << "_g" << constants::GENERATION
            << "_pM" << constants::PROB_MUTATION
            << "_pC" << constants::PROB_CROSSOVER
            << "_el" << constants::ELITISM_FRACTION
            << "_bias" << constants::BIAS
            << "_prox" << constants::PROXIMITY_SIGHT
            << "_trials" << constants::N_TRIAL;
    return metainfo.str();
}
