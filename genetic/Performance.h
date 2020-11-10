//
// Created by Andrea Petreti on 10/11/2020.
//

#ifndef SWARM_GEN_PERFORMANCE_H
#define SWARM_GEN_PERFORMANCE_H

#include <ga/ga.h>
#include <ga/GAGenome.h>
#include <ga/GA1DBinStrGenome.h>
#include <ga/GA1DArrayGenome.C>
#include <iostream>

using namespace std;

namespace performance {

    class PerformanceLog {

    public:
        PerformanceLog(const char* folder, const char* fileMetaInfo);
        void writePerformance(GAPopulation& population);
        void close();

    private:
        void saveBestOfGeneration(int generation, GAGenome& genome);
        string folder;
        ofstream file;
    };

    PerformanceLog::PerformanceLog(const char *folder, const char* fileMetaInfo) : folder(folder) {
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        time (&rawtime);
        timeinfo = localtime (&rawtime);

        strftime (buffer,80,"%m-%d_%H-%M",timeinfo  );

        file.open(this->folder + string(buffer) + fileMetaInfo + ".csv");
        file << "generation;score;genome;bn" << endl << flush;
    }

    void PerformanceLog::writePerformance(GAPopulation& population) {
        for(int i = 0; i < population.size(); i++) {
            file << population.geneticAlgorithm()->generation() << ";"
                << population.individual(i).score() << ";"
                << population.individual(i) << ";"
                << endl << flush;
        }
        saveBestOfGeneration(population.geneticAlgorithm()->generation(), population.best(0));
    }

    void PerformanceLog::saveBestOfGeneration(int generation, GAGenome &genome) {
        ofstream bestFile;
        bestFile.open(string(folder) + "best_gen_" + to_string(generation) + ".csv");
        bestFile << "score;genome;bn" << endl;
        bestFile << genome.score() << ";" << genome << endl << flush;
        bestFile.close();
    }

    void PerformanceLog::close() {
        file.flush();
        file.close();
    }

}

#endif //SWARM_GEN_PERFORMANCE_H
