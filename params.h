//
// Created by Andrea Petreti on 03/11/2020.
//
#include <argos3/core/utility/math/vector2.h>

namespace constants {
    extern const argos::CVector3 CIRCLE1(-1.83f, 0.0f, 0.67f);
    extern const argos::CVector3 CIRCLE2(1.83f, 0.0f, 0.67f);

    extern const int N_FOOTBOT = 50;
    extern const float FOOTBOT_RADIUS = 0.1f;

    extern const float ARENA_SIDE_SIZE = 6.0f - 0.1f; // arena size - wall depth
    extern const int MAX_ATTEMPTS_LOCATION_SPAWN = 20;
}

#define N_TRIAL 3 // number of experiment for each individual
#define N_BOT 20

/* Boolean network params */
#define TOTAL_NODE_COUNT 20 // it include input and output node count
#define INPUT_NODE_COUNT 12 // 8 (proximity) + 4 (ground)
#define OUTPUT_NODE_COUNT 2 // left and right wheel

/* Genetic parameters */
#define POPULATION 8
#define N_GENERATIONS 4

/* Environment default params */
#define CIRCLE_RADIUS 0.7f