//
// Created by Andrea Petreti on 10/11/2020.
//

#ifndef SWARM_GEN_BNGENOME_H
#define SWARM_GEN_BNGENOME_H

#include <ga/GAGenome.h>
#include <ga/GA1DBinStrGenome.h>
#include <utility/NewRandom.h>
#include <argos3/core/simulator/simulator.h>
#include <loop-functions/evolution/evolution_loop.h>

namespace bngenome {

    typedef struct {
        argos::CSimulator* simulator;
        EvolutionLoop* loop;
        int nTrials;
    } Experiment;

    GAGenome::Initializer genomeInitializer(float bias, newrandom::Random *rnd);
    float genomeEvaluator(GAGenome& genome);

    static function<void(GAGenome&)> functionInitializerCallback;
    static void initializer(GAGenome& genome) { functionInitializerCallback(genome); }
}

#endif //SWARM_GEN_BNGENOME_H
