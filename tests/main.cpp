
#include <fplus/fplus.hpp>
#include <constants.h>
#include <iostream>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop-functions/evolution/evolution_loop.h>
#include <chrono>
#include <ga/ga.h>

int main() {

    static argos::CSimulator& simulator = argos::CSimulator::GetInstance();

    simulator.SetExperimentFileName("tests/ev-test-gui.argos");
    simulator.LoadExperiment();

    static EvolutionLoop* loop = dynamic_cast<EvolutionLoop*>(&simulator.GetLoopFunctions());

    loop->GenerateRandomSpawnLocation(3);

    simulator.Reset();
    loop->PrepareForTrial(1);
    simulator.Execute();

    cout << "Performance " << loop->CalculatePerformance() << endl;
    simulator.Destroy();
}
