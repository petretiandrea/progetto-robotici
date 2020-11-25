//
// Created by Andrea Petreti on 09/11/2020.
//
#include <constants.h>
#include <utility/Utility.h>
#include <genetic/parallel/ParallelEvaluator.h>
#include <common/BNGenome.h>
#include <common/FileLogger.h>


#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop-functions/evolution/evolution_loop.h>
#include <ga/ga.h>
#include <ga/GA1DBinStrGenome.h>
#include <ga/GA1DArrayGenome.C>
#include "ParallelEvaluator.h"

int main() {

    performance::FileLogger logger("performance/", "evolve_" + utility::createMetaInfoFilename());
    ParallelEvaluator evaluator(logger,
                                constants::GENOME_SIZE,
                                constants::POPULATION + constants::REPLACEMENT,
                                constants::N_TRIAL,
                                15);

    auto rnd = newrandom::Random(constants::RANDOM_SEED);

    GA1DBinaryStringGenome genome(constants::GENOME_SIZE);
    genome.initializer(bngenome::genomeInitializer(constants::BIAS, &rnd));

    GAPopulation population(genome);
    population.userData(&evaluator);
    population.evaluator(ParallelEvaluator::populationEvaluator);

    //GARealGenome genome(8, allele, evaluate);
    GASteadyStateGA ga(population);
    ga.maximize();
    ga.populationSize(constants::POPULATION);
    ga.nBestGenomes(0);
    ga.nReplacement(constants::REPLACEMENT);
    ga.nGenerations(constants::GENERATION);
    ga.pMutation(constants::PROB_MUTATION);
    ga.pCrossover(constants::PROB_CROSSOVER);
    ga.flushFrequency(0);

    ga.initialize(1234);

    do {
        ga.step();
    } while (!ga.done());

    logger.close();

    return 0;
}