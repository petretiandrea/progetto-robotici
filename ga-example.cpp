#include <iostream>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop-functions/evolution/evolution_loop.h>

#include <ga/ga.h>
#include <ga/GAAllele.h>
#include <ga/GAAllele.C>


void LaunchARGoS() {
    static argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();

    //argos::CLuaController

    for (int i = 0; i < 2; i++) {
        argos::LOG << "Start trial: "<< i << "..." << std::endl;

        cSimulator.Reset();
        cSimulator.Execute();
    }
}

float evaluate(GAGenome& genome) {
    auto gen = dynamic_cast<GA2DArrayAlleleGenome<int>&>(genome);

    std::cout << "{ ";
    for (int i = 0; i < gen.size(); i++) {
        std::cout << gen[i] << ",";
    }
    std::cout << "}" << std::endl << std::endl;

    return 0.5;
}

const unsigned int ELITISM_FRACTION = 5;

int main() {

    /*argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

    cSimulator.SetExperimentFileName("tests/ev-test.argos");
    cSimulator.LoadExperiment();

    LaunchARGoS();

    cSimulator.Destroy();
*/

    // try genetic algo
    int enumeration[] = {0, 1};
    GAAlleleSet<int> allele(2, enumeration);
    std::cout << allele.type() << std::endl;
    GA2DArrayAlleleGenome<int> genome(25, 8, allele, evaluate);
    //GARealGenome genome(8, allele, evaluate);
    GASteadyStateGA ga(genome);


    ga.maximize();
    ga.populationSize(4);
    ga.pCrossover(1);
    ga.pMutation(1/4.0);
    ga.nGenerations(2);
    ga.nReplacement(4-(4/ELITISM_FRACTION));
    ga.flushFrequency(0);

    ga.initialize(1234);

    do {
        ga.step();
        std::cout << "Generation #" << ga.generation() << " DONE" << std::endl;
        auto individual = ga.statistics().bestIndividual();
        std::cout << "Best score of " << ": " << individual.score() << std::endl;
        std::cout << std::endl;

    } while(!ga.done());

    return 0;
}
