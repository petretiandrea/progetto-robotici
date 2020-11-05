
#include <constants.h>
#include <iostream>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop-functions/evolution/evolution_loop.h>

#include <ga/ga.h>
#include <ga/GAAllele.h>
#include <ga/GAAllele.C>

struct GenerationMetadata {
    int individualId;
};

float evaluate(GAGenome& genome) {
    auto& boolGenome = dynamic_cast<GA1DArrayGenome<bool>&>(genome);
    auto* metadata = static_cast<GenerationMetadata*>(boolGenome.userData());
    std::cout << "Starting evaluation for genome" << metadata->individualId << " of generation " << boolGenome.geneticAlgorithm()->generation() << std::endl;

    static argos::CSimulator& simulator = argos::CSimulator::GetInstance();
    static auto& loopFunctions = dynamic_cast<EvolutionLoop&>(simulator.GetLoopFunctions());

    double performanceSum = 0;
    for (int i = 0; i < constants::N_TRIAL; i++) {
        std::cout << "\tStart trial: "<< i << "..." << std::endl;

        /*
         * 1. reset the simulator
         * 2. prepare the loop function for ith trial
         * 4. configure all controllers from genome
         * 3. execute the simulation
         */

        simulator.Reset();
        loopFunctions.PrepareForTrial(i);
        loopFunctions.ConfigureFromGenome(boolGenome);
        simulator.Execute();
        auto performance = loopFunctions.CalculatePerformance();
        std::cout << "\tPerformance: "<< performance << std::endl;
        performanceSum += performance;
    }

    metadata->individualId += 1;

    return (float)performanceSum / constants::N_TRIAL;
}

int main() {
    static argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

    cSimulator.SetExperimentFileName("tests/ev-test.argos");
    cSimulator.LoadExperiment();

    // setup genetic algo
    auto* metadata = new GenerationMetadata { 0 };
    bool enumeration[] = {false, true};
    GAAlleleSet<bool> allele(2, enumeration);
    GA1DArrayAlleleGenome<bool> genome(25 * 8, allele, evaluate, metadata);
    //GARealGenome genome(8, allele, evaluate);
    GASteadyStateGA ga(genome);
    ga.maximize();
    ga.nReplacement(constants::REPLACEMENT);
    ga.populationSize(constants::POPULATION);
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

    ga.initialize(1245);

    do {
        metadata->individualId = 0;
        ga.step();
        std::cout << "Generation # " << ga.generation() << " completed!" << std::endl;
    } while (!ga.done());

    cSimulator.Destroy();


    return 0;
}