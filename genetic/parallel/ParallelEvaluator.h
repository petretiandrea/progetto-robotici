//
// Created by Andrea Petreti on 24/11/2020.
//

#ifndef SWARM_GEN_PARALLELEVALUATOR_H
#define SWARM_GEN_PARALLELEVALUATOR_H

#include <sys/types.h>
#include "SharedMemory.h"
#include <vector>
#include <ga/ga.h>
#include <common/FileLogger.h>


class ParallelEvaluator {

    // useful for check if a genome is already evaluated.
    class EvaluatedHack : public GAGenome {
        public:
            static bool isEvaluated(GAGenome* genome) {
                auto a = (EvaluatedHack*)(genome);
                return a->_evaluated;
            }
    };

public:
    ParallelEvaluator(performance::FileLogger& log, int genomeSize, int population, int nTrials, int parallelism);
    ~ParallelEvaluator();

    static void populationEvaluator(GAPopulation& population);
    inline int parallelism() { return slavePids.size(); }

public:
    performance::FileLogger& log;
    SharedMemory* sharedMemory;
    std::vector<pid_t> slavePids;
    void prepareTaskSlave(GAPopulation& population);
    void resumeSlaves();
    void waitSlaves();

    [[noreturn]] void slaveBehaviour(int slaveId, int nTrials) const;
    void evaluatePopulation(GAPopulation& population);

private:
    static float best_score;
    static GAGenome* best_genome;

};

#endif //SWARM_GEN_PARALLELEVALUATOR_H
