//
// Created by Andrea Petreti on 09/11/2020.
//

#include "ParallelEvaluator.h"
#include <constants.h>
#include <genetic/BNGenome.h>
#include <genetic/Performance.h>

int main() {

    ostringstream metainfo;
    metainfo
        << "_p" << constants::POPULATION
        << "_g" << constants::GENERATION
        << "_pM" << constants::PROB_MUTATION
        << "_pC" << constants::PROB_CROSSOVER
        << "_el" << constants::ELITISM_FRACTION
        << "_bias" << constants::BIAS
        << "_prox" << constants::PROXIMITY_SIGHT;

    performance::PerformanceLog logger("performance/", metainfo.str().c_str());
    ParallelEvaluator evaluator(logger,
                                constants::GENOME_SIZE,
                                constants::POPULATION + constants::REPLACEMENT,
                                constants::N_TRIAL,
                                8);

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
