//
// Created by Andrea Petreti on 10/11/2020.
//

#include "BNGenome.h"

using namespace bngenome;

GAGenome::Initializer bngenome::genomeInitializer(float bias, newrandom::Random *rnd) {
    functionInitializerCallback = [bias, rnd](GAGenome& genome) -> void {
        auto& arrayGenome = ((GA1DBinaryStringGenome&) genome);
        auto initializer = BooleanNetwork::booleanFunctionsInitializer(rnd, bias);

        for(int i = 0; i < arrayGenome.size(); i++) {
            arrayGenome.gene(i, initializer());
        }
        arrayGenome.evalData(CustomEvalData()); // set empty eval data
    };
    return initializer;
}

bool debug = false;

float bngenome::genomeEvaluator(GAGenome& genome) {
    auto& boolGenome = dynamic_cast<GA1DBinaryStringGenome&>(genome);
    auto& experiment = *static_cast<Experiment*>(boolGenome.userData());
    auto* evalData = dynamic_cast<CustomEvalData*>(boolGenome.evalData());

    double performanceSum = 0;
    double robotCount = 0;
    experiment.loop->ConfigureFromGenome(boolGenome);

    for (int i = 0; i < experiment.nTrials; i++) {
        //cout << "\tStart trial "<< i << "...";

        /*
         * 1. reset the simulator
         * 2. prepare the loop function for ith trial
         * 4. configure all controllers from genome
         * 3. execute the simulation
         */
        experiment.loop->PrepareForTrial(i);
        experiment.simulator->Reset();
        experiment.simulator->Reset();
        experiment.simulator->Execute();
        auto performance = experiment.loop->CalculatePerformance();
        //std::cout << " performance: "<< performance << std::endl;
        performanceSum += performance;
        robotCount += experiment.loop->MaxRobotCount();
    }

    evalData->robotCount = robotCount / (double) experiment.nTrials;

    std::cout << "Avg robot count "  << evalData->robotCount << " for " << boolGenome << std::endl;
    return (float) (performanceSum / (double) experiment.nTrials);
}