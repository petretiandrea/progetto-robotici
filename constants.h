//
// Created by Andrea Petreti on 03/11/2020.
//
#include <cmath>
#include <argos3/core/utility/math/vector3.h>

namespace constants {
    /* Boolean Network controller params */
    extern const int NODES = 50;
    extern const int INPUT_FOR_NODE = 3;
    extern const float BIAS = 0.21f;
    extern const float PROXIMITY_SIGHT = 3.0f * 0.0f; // threshold or sight of proximity sensor (8 virtual sensor from 24 => 3.0 is the value)
    extern const float MOTOR_GROUND_BLACK_THRESHOLD = 0.1f; // threshold to consider ground as black.
    extern const float CONSTANT_SPEED_OUTPUT = 10.0f; // speed output
    extern const int RANDOM_SEED = 123;

    /* Genetic algorithm constants */
    extern const int N_TRIAL = 3; // number of experiment for each individual
    extern const int GENOME_SIZE = (int) (NODES * pow(2, INPUT_FOR_NODE)); // length of boolean function
    extern const int POPULATION = 100;
    extern const int GENERATION = 100;
    extern const float PROB_MUTATION = 0.03f;
    extern const float PROB_CROSSOVER = 0.1f;
    extern const int ELITISM_FRACTION = 5;
    extern const int REPLACEMENT = (int)(POPULATION - (POPULATION / (float)ELITISM_FRACTION));

    /* Environment constants */
    extern const int N_FOOTBOT = 30;
    extern const float FOOTBOT_RADIUS = 0.1f;
    extern const int MAX_ATTEMPTS_LOCATION_SPAWN = 20;  // number of max attempts for locate randomly the bot
//    extern const float ARENA_SIDE_SIZE = 6.0f - 0.1f;   // arena size - wall depth
//    extern const argos::CVector3 CIRCLE1(-1.83f, 0.0f, 0.8f);  // location of first black circle
//    extern const argos::CVector3 CIRCLE2(1.83f, 0.0f, 0.8f);   // location of second black circle
    extern const float ARENA_SIDE_SIZE = 5.0f - 0.1f;   // arena size - wall depth
    extern const argos::CVector3 CIRCLE1(-1.5f, 0.0f, 0.8f);  // location of first black circle
    extern const argos::CVector3 CIRCLE2(1.5f, 0.0f, 0.8f);   // location of second black circle
}