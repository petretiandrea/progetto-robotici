//
// Created by Andrea Petreti on 24/11/2020.
//

#include "ParallelEvaluator.h"
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <csignal>
#include <argos3/core/simulator/simulator.h>
#include <loop-functions/evolution/evolution_loop.h>
#include <common/BNGenome.h>
#include <controllers/rbn-controller/serializer/BooleanNetworkParser.h>

ParallelEvaluator::ParallelEvaluator(performance::FileLogger &log, int genomeSize, int population, int nTrials,
                                     int parallelism) : log(log) {

    sharedMemory = new SharedMemory("shared_mem2", genomeSize, population, parallelism);

    for (int i = 0; i < parallelism; i++) {
        slavePids.push_back(::fork());
        if (slavePids.back() == 0) {
            slaveBehaviour(i, nTrials);
        } else if(slavePids.back() < 0) {
            cerr << "Error creating slave " << endl;
        }
    }

    ::sleep(3);
}

void ParallelEvaluator::evaluatePopulation(GAPopulation &population) {
    cout << "Evaluating generation #" << population.geneticAlgorithm()->generation() << endl;
    // load genome to shared memory
    for(int i = 0; i < population.size(); i++) {
        auto& boolGenome = dynamic_cast<GA1DBinaryStringGenome&>(population.individual(i));
        sharedMemory->SetGenome(i, boolGenome);
        sharedMemory->SetScore(i, ParallelEvaluator::EvaluatedHack::isEvaluated(&boolGenome) ? boolGenome.score() : -1);
    }

    prepareTaskSlave(population);
    resumeSlaves();
    waitSlaves();

    for(int i = 0; i < population.size(); i++) {
        auto score = sharedMemory->GetScore(i);
        population.individual(i).score(score);
    }
    cout << "Generation # " << population.geneticAlgorithm()->generation() << " completed!" << endl;
}

void ParallelEvaluator::prepareTaskSlave(GAPopulation &population) {
    auto populationSlice = population.size() / parallelism();
    auto populationOvers = population.size() % parallelism();
    for(int i = 0; i < parallelism(); i++) {
        auto populationIndexStart = (i * populationSlice);
        // assign to last one the leftovers population
        auto size = (populationOvers > 0 && i == parallelism() - 1) ?
                    populationSlice + populationOvers :
                    populationSlice;
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
            cerr << "[FATAL] Slave process with PID " << slavePid
                   << " exited, can't continue. Check file ARGoS_LOGERR_" << slavePid << " for more information."
                   << std::endl;
            ::exit(1);
        }
    }
}

[[noreturn]] void ParallelEvaluator::slaveBehaviour(int slaveId, int nTrials) const {

    //::signal(SIGTERM)

    std::ofstream cLOGFile(std::string("logs/ARGoS_LOG_" + ToString(::getpid())).c_str(), std::ios::out);
    LOG.DisableColoredOutput();
    LOG.GetStream().rdbuf(cLOGFile.rdbuf());
    std::ofstream cLOGERRFile(std::string("logs/ARGoS_LOGERR_" + ToString(::getpid())).c_str(), std::ios::out);
    LOGERR.DisableColoredOutput();
    LOGERR.GetStream().rdbuf(cLOGERRFile.rdbuf());

    argos::CSimulator& simulator = argos::CSimulator::GetInstance();
    cout << " Slave " << slaveId << " pid: " << ::getpid() << " sim " << &simulator << endl;

    try {
        simulator.SetExperimentFileName("genetic/experiment.argos");
        /* Load it to configure ARGoS */
        simulator.LoadExperiment();
        LOG.Flush();
        LOGERR.Flush();
    } catch (CARGoSException& ex) {
        cerr << ex.what() << endl;
        ::raise(SIGTERM);
    }

    EvolutionLoop& loop = dynamic_cast<EvolutionLoop&>(simulator.GetLoopFunctions());
    loop.GenerateRandomSpawnLocation(nTrials);
    //cout << " Slave " << slaveId << " pid: " << ::getpid() << " loop " << &loop << " trials " << nTrials << endl;

    bngenome::Experiment experiment = {
            .simulator = &simulator,
            .loop = &loop,
            .nTrials = nTrials
    };

    while (true) {
        ::raise(SIGSTOP);
        auto* slices = sharedMemory->GetSlice(slaveId);
        LOG << " Slave " << slaveId << " pid: " << ::getpid() << " slice " << slices[0] << " to " << (slices[0] + slices[1]) << " size: " << slices[1] << endl;

        for(int i = slices[0]; i < slices[0] + slices[1]; i++) {
            GA1DBinaryStringGenome* genome = sharedMemory->GetGenome(i);
            genome->userData(&experiment);

            bool needEvaluation = sharedMemory->GetScore(i) == -1;
            if(needEvaluation) {
                auto performance = bngenome::genomeEvaluator(*genome);
                sharedMemory->SetScore(i, performance);
            }

            //LOG << "Slave " << slaveId << " Perf " << sharedMemory->GetScore(i) << " gen " << *genome << endl;

            delete genome;
        }
        delete[] slices;
        LOG.Flush();
        LOGERR.Flush();
    }
}

float ParallelEvaluator::best_score = 0;
GAGenome* ParallelEvaluator::best_genome = nullptr;

void ParallelEvaluator::populationEvaluator(GAPopulation &population) {
    auto* evaluator = static_cast<ParallelEvaluator*>(population.userData());
    evaluator->evaluatePopulation(population);

    for(int i = 0; i < population.size(); i++) {
        auto score = population.individual(i).score();
        if(score >= best_score) {
            best_genome = &population.individual(i);
            best_score = best_genome->score();
            evaluator->log.saveGenomeAsBest(*best_genome);
        }
    }

    evaluator->log.saveGenerationPerformance(population);
}

ParallelEvaluator::~ParallelEvaluator() {
    for(auto slavePid : slavePids) {
        ::kill(slavePid, SIGKILL);
    }
    delete sharedMemory;
}
