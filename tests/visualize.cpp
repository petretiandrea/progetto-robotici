//
// Created by Andrea Petreti on 12/11/2020.
//

#include <constants.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop-functions/evolution/evolution_loop.h>
#include <ga/ga.h>

/**
 * Execute argos in visual mode. It load the controller from experiment.
 */
int main() {

    static argos::CSimulator& simulator = argos::CSimulator::GetInstance();

    simulator.SetExperimentFileName("tests/ev-test-gui.argos");
    simulator.LoadExperiment();

    static EvolutionLoop* loop = dynamic_cast<EvolutionLoop*>(&simulator.GetLoopFunctions());

    loop->GenerateRandomSpawnLocation(1);

    simulator.Reset();
    loop->PrepareForTrial(0);
    simulator.Execute();
    simulator.Destroy();
}
