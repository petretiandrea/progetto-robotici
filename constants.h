//
// Created by Andrea Petreti on 03/11/2020.
//
#include <argos3/core/utility/math/vector3.h>

namespace constants {
    /* Genetic algorithm constants */
    extern const int N_TRIAL = 2; // number of experiment for each individual
    extern const int POPULATION = 50;

    /* Environment constants */
    extern const int N_FOOTBOT = 50;
    extern const float FOOTBOT_RADIUS = 0.1f;
    extern const float ARENA_SIDE_SIZE = 6.0f - 0.1f;   // arena size - wall depth
    extern const int MAX_ATTEMPTS_LOCATION_SPAWN = 20;  // number of max attempts for locate randomly the bot
    extern const argos::CVector3 CIRCLE1(-1.83f, 0.0f, 0.67f);  // location of first black circle
    extern const argos::CVector3 CIRCLE2(1.83f, 0.0f, 0.67f);   // location of second black circle

    /*
     * #define TOTAL_NODE_COUNT 20 // it include input and output node count
     * #define INPUT_NODE_COUNT 12 // 8 (proximity) + 4 (ground)
     * #define OUTPUT_NODE_COUNT 2 // left and right wheel
     */
}