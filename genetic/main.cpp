
#include <constants.h>
#include <iostream>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop-functions/evolution/evolution_loop.h>
#include <argos3/core/wrappers/lua/lua_controller.h>
#include "controllers/rbn_controller.h"

#include <ga/ga.h>
#include <ga/GAAllele.h>
#include <ga/GAAllele.C>
#include <loop-functions/evolution/Circle.h>

void LaunchARGoS(argos::CSimulator& simulator) {

    static auto& loopFunctions = dynamic_cast<EvolutionLoop&>(simulator.GetLoopFunctions());

    //argos::CLuaController

    for (int i = 0; i < constants::N_TRIAL; i++) {
        argos::LOG << "Start trial: "<< i << "..." << std::endl;

        /*
         * 1. reset the simulator
         * 2. prepare the loop function for ith trial
         * 3. execute the simulation
         */

        simulator.Reset();

        loopFunctions.PrepareForTrial(i);

        simulator.Execute();
    }
}

int main() {

    static argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();

    cSimulator.SetExperimentFileName("tests/ev-test.argos");
    cSimulator.LoadExperiment();

    LaunchARGoS(cSimulator);

    cSimulator.Destroy();


    return 0;
}