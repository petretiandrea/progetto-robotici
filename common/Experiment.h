//
// Created by Andrea Petreti on 27/11/2020.
//

#ifndef SWARM_GEN_EXPERIMENT_H
#define SWARM_GEN_EXPERIMENT_H

#include <argos3/core/simulator/simulator.h>
#include <loop-functions/evolution/evolution_loop.h>

namespace experiment {

    typedef struct {
        argos::CSimulator* simulator;
        EvolutionLoop* loop;
        int nTrials;
    } Experiment;

    //extern float evaluateExperiment(const Experiment& experiment);

}

#endif //SWARM_GEN_EXPERIMENT_H
