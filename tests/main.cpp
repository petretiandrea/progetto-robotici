
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

    simulator.SetExperimentFileName("tests/ev-test.argos");
    simulator.LoadExperiment();

    static EvolutionLoop* loop = dynamic_cast<EvolutionLoop*>(&simulator.GetLoopFunctions());

    const char* genomeString = "0100101100011110000100001001000101110001100000000001100000000001001001000100010010000000001100100001000101000000100000010001000010110000100000010000100110110010";

    GA1DBinaryStringGenome genome(constants::GENOME_SIZE);
    for(int i = 0; i < genome.size(); i++) {
        genome.gene(i, genomeString[i] == '0' ? 0 : 1);
    }

    loop->GenerateRandomSpawnLocation(1);

    loop->Reset();
    loop->PrepareForTrial(0);
    loop->ConfigureFromGenome(genome);

    simulator.Execute();
    simulator.Destroy();
}
