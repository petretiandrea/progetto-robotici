//
// Created by Andrea Petreti on 09/11/2020.
//

#ifndef SWARM_GEN_PARALLELEVALUATOR_H
#define SWARM_GEN_PARALLELEVALUATOR_H

#include <fplus/fplus.hpp>
#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop-functions/evolution/evolution_loop.h>
#include <genetic/BNGenome.h>
#include <ga/ga.h>
#include <ga/GA1DBinStrGenome.h>
#include <ga/GA1DArrayGenome.C>
#include "SharedMemory.h"
#include <genetic/Performance.h>

using namespace std;
using namespace argos;

class ParallelEvaluator {

public:
    ParallelEvaluator(performance::PerformanceLog& log, int genomeSize, int population, int nTrials, int parallelism);
    ~ParallelEvaluator();

    static void populationEvaluator(GAPopulation& population);

    int parallelism() { return slavePids.size(); }

private:
    void evaluatePopulation(GAPopulation &population);

private:
    performance::PerformanceLog& log;
    void prepareTaskSlave(GAPopulation& population);
    void resumeSlaves();
    void waitSlaves();

    // slave behaviour
    [[noreturn]] void launchSlaveArgos(int slaveId, int nTrials);

private:
    int basePopulation;
    SharedMemory* sharedMemory;
    std::vector<pid_t> slavePids;
};

void handleSlaveTermination(int slavePid);

ParallelEvaluator::ParallelEvaluator(performance::PerformanceLog& log, int genomeSize, int population, int nTrials, int parallelism) :
    log(log),
    basePopulation(population) {

    sharedMemory = new SharedMemory("shared_mem", genomeSize, population, parallelism);

    // initialize parallel process
    for (int i = 0; i < parallelism; i++) {
        slavePids.push_back(::fork());
        if (slavePids.back() == 0) {
            launchSlaveArgos(i, nTrials);
        }
    }

    ::sleep(3);
}

void ParallelEvaluator::evaluatePopulation(GAPopulation &population) {
    cout << "Evaluating generation #" << population.geneticAlgorithm()->generation() << endl;
    
    // load genome on shared memory
    for(int i = 0; i < population.size(); i++) {
        auto &boolGenome = dynamic_cast<GA1DBinaryStringGenome &>(population.individual(i));
        sharedMemory->SetGenome(i, boolGenome);
    }

    prepareTaskSlave(population);
    resumeSlaves();
    waitSlaves();

    // retrieve performance from shared memory
    for(int i = 0; i < population.size(); i++) {
        auto score = sharedMemory->GetScore(i);
        population.individual(i).score((float) score);
    }
    cout << "Generation # " << population.geneticAlgorithm()->generation() << " completed!" << endl;
}

float best_score = 0;
GAGenome* best_genome;

void ParallelEvaluator::populationEvaluator(GAPopulation& population) {
    auto* evaluator = (ParallelEvaluator*)population.userData();
    evaluator->evaluatePopulation(population);

    for(int i = 0; i < population.size(); i++) {
        auto score = population.individual(i).score();
        if(score > best_score) {
            best_genome = &population.individual(i);
            best_score = best_genome->score();
            evaluator->log.saveBest(*best_genome);
        }
    }

    evaluator->log.writePerformance(population);
}

void ParallelEvaluator::prepareTaskSlave(GAPopulation& population) {
    int populationToEvaluateOffset = 0;
    int populationToEvaluate = population.size();

    // TODO: workaround for prevent twice evaluation of genomes
    if(population.geneticAlgorithm()->classID() == GAID::SteadyStateGA &&
        population.size() == basePopulation) {
        auto* steadyState = dynamic_cast<GASteadyStateGA*>(population.geneticAlgorithm());
        
        populationToEvaluateOffset = population.size() - steadyState->nReplacement() - 1;
        if(populationToEvaluateOffset > 0) {
            populationToEvaluate = steadyState->nReplacement();
        }
    }

    auto populationSlice = populationToEvaluate / parallelism();
    auto populationOvers = populationToEvaluate % parallelism();

    for(int i = 0; i < parallelism(); i++) {
        auto populationIndexStart = (i * populationSlice) + populationToEvaluateOffset;
        // assign to last one the leftovers population
        auto size = (populationOvers > 0 && i == parallelism() - 1) ?
                populationSlice + populationOvers :
                populationSlice;
        cout << "Slice " << populationIndexStart << " s " << size << endl;
        sharedMemory->SetSlice(i, populationIndexStart, size);
    }
}

void ParallelEvaluator::resumeSlaves() {
    for(int i = 0; i < parallelism(); i++ ) {
        // resume child process
        ::kill(slavePids[i], SIGCONT);
    }
}

void ParallelEvaluator::waitSlaves() {
    // wait slave to finish
    int slaveInfo;
    pid_t slavePid;
    for(int waitSize = slavePids.size(); waitSize > 0; waitSize--) {
        slavePid = ::waitpid(-1, &slaveInfo, WUNTRACED);
        if (!WIFSTOPPED(slaveInfo)) {
            LOGERR << "[FATAL] Slave process with PID " << slavePid
                   << " exited, can't continue. Check file ARGoS_LOGERR_" << slavePid << " for more information."
                   << std::endl;
            ::exit(1);
        }
    }
}

/** Slaves Behaviour */
[[noreturn]] void ParallelEvaluator::launchSlaveArgos(int slaveId,
                                                      int nTrials) {

    ::signal(SIGTERM, handleSlaveTermination);

    argos::CSimulator& simulator = argos::CSimulator::GetInstance();
    try {
        simulator.SetExperimentFileName("tests/ev-test.argos"); //TODO:
        simulator.LoadExperiment();
    } catch (CARGoSException& ex) {
        cerr << "Failed to launch argos " << ex.what() << endl;
        ::raise(SIGTERM);
    }

    auto& loop = dynamic_cast<EvolutionLoop&>(simulator.GetLoopFunctions());
    auto experiment = bngenome::Experiment { .simulator = &simulator, .loop = &loop, .nTrials = nTrials };
    loop.GenerateRandomSpawnLocation(nTrials);

    while(true) {
        //cout << "Slave " << slaveId << " waiting master..." << endl;
        ::raise(SIGSTOP);
        auto* slices = sharedMemory->GetSlice(slaveId);
        //cout << "Slave " << slaveId << " started from " << slices[0] << endl;

        for(int i = slices[0]; i < slices[0] + slices[1]; i++) {
            //cout << "\tEvaluating genome " << i << endl;
            auto individual = sharedMemory->GetGenome(i);
            individual->userData(&experiment);

            auto performance = bngenome::genomeEvaluator(*individual);

            sharedMemory->SetScore(i, performance);

            delete individual;
        }

        delete[] slices;
    }
}

ParallelEvaluator::~ParallelEvaluator() {
    /* Terminate slaves */
    for(auto slavePid : slavePids) {
        ::kill(slavePid, SIGTERM);
    }
    delete sharedMemory;
}

void handleSlaveTermination(int slavePid) {
    argos::CSimulator::GetInstance().Destroy();
}

#endif //SWARM_GEN_PARALLELEVALUATOR_H
