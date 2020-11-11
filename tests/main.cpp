
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

    const char* genomeString = "11010011001111111100110000100001000010000001001100111111100110100111110010111111110101001001100000100101001001101001110011111110011000001101000110100011011111010101000100000100001010111111000011100100";
    GA1DBinaryStringGenome genome(constants::GENOME_SIZE);
    for(int i = 0; i < genome.size(); i++) {
        genome.gene(i, genomeString[i] == '0' ? 0 : 1);
    }

    loop->GenerateRandomSpawnLocation(3);

    loop->Reset();
    loop->PrepareForTrial(1);
    loop->ConfigureFromGenome(genome);

    simulator.Execute();

    cout << "Performance " << loop->CalculatePerformance() << endl;
    simulator.Destroy();
}
