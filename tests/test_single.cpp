
#include <constants.h>
#include <iostream>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop-functions/evolution/evolution_loop.h>
#include <chrono>
#include <ga/ga.h>
#include <genetic/Performance.h>
#include <utility/Utility.h>

static performance::PerformanceLog logger("performance/", "test_" + utility::createMetaInfoFilename());

int main() {

    static argos::CSimulator& simulator = argos::CSimulator::GetInstance();

    simulator.SetExperimentFileName("tests/ev-test.argos");
    simulator.LoadExperiment();

    static EvolutionLoop* loop = dynamic_cast<EvolutionLoop*>(&simulator.GetLoopFunctions());

    loop->GenerateRandomSpawnLocation(constants::N_TRIAL);

    double performance[constants::N_TRIAL];
    for(int i = 0; i < constants::N_TRIAL; i++) {

        simulator.Reset();
        loop->PrepareForTrial(i);
        simulator.Execute();
        loop->CalculatePerformance();

        auto score = loop->CalculatePerformance();
        cout << "Performance " <<  score << endl;
        performance[i] = score;
    }

    logger.saveTrialsPerformance(constants::N_TRIAL, performance);
    logger.close();

    simulator.Destroy();
}
