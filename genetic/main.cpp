
#include <fplus/fplus.hpp>
#include <constants.h>
#include <iostream>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop-functions/evolution/evolution_loop.h>
#include <chrono>
#include "BNGenome.h"
#include "Performance.h"

using namespace std::chrono;

#include <ga/ga.h>
#include <ga/GAAllele.h>
#include <ga/GAAllele.C>

static performance::PerformanceLog logger("performance/sequential/", "");

void evaluatePopulation(GAPopulation& population) {

    cout << "Evaluating generation #" << population.geneticAlgorithm()->generation() << endl;
    for(int i = 0; i < population.size(); i++) {
        cout << "\tEvaluating genome " << i << endl;
        population.individual(i).evaluate();
    }
    cout << "Generation # " << population.geneticAlgorithm()->generation() << " completed!" << endl;

    logger.writePerformance(population);
}

int main() {

    static argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

    cSimulator.SetExperimentFileName("tests/ev-test.argos");
    cSimulator.LoadExperiment();

    auto rnd = new newrandom::Random(constants::RANDOM_SEED);

    // setup genetic algo
    auto experiment = bngenome::Experiment {
        .simulator = &cSimulator,
        .loop = dynamic_cast<EvolutionLoop*>(&cSimulator.GetLoopFunctions()),
        .nTrials = constants::N_TRIAL
    };

    experiment.loop->GenerateRandomSpawnLocation(constants::N_TRIAL);

    GA1DBinaryStringGenome genome1(constants::GENOME_SIZE, bngenome::genomeEvaluator, &experiment);
    genome1.initializer(bngenome::genomeInitializer(constants::BIAS, rnd));

    GAPopulation population(genome1);
    population.userData(&experiment);
    population.evaluator(evaluatePopulation);

    //GARealGenome genome(8, allele, evaluate);
    GASteadyStateGA ga(population);
    ga.maximize();
    ga.populationSize(constants::POPULATION);
    ga.nReplacement(constants::REPLACEMENT);
    ga.nGenerations(constants::GENERATION);
    ga.pMutation(constants::PROB_MUTATION);
    ga.pCrossover(constants::PROB_CROSSOVER);
    ga.flushFrequency(0);

    std::cout << "Initializing genetic..." << std::endl;
    std::cout <<"NUM OF BEST IS: " << ga.nBestGenomes() << std::endl;
    std::cout <<"pCrossover is: " << ga.pCrossover() << std::endl;
    std::cout <<"pMutation is: " << ga.pMutation() << std::endl;
    std::cout <<"num of replacement is: " << ga.nReplacement() << std::endl;
    std::cout <<"NUM of pop is: " << ga.populationSize() << std::endl;
    std::cout <<"cga freq is : " << ga.flushFrequency() << std::endl;

    ga.initialize(1234);

    do {
        ga.step();
    } while (!ga.done());

    logger.close();

    cSimulator.Destroy();

    return 0;
}