//
// Created by Andrea Petreti on 10/11/2020.
//

#ifndef SWARM_GEN_FILELOGGER_H
#define SWARM_GEN_FILELOGGER_H

#include <ga/ga.h>
#include <ga/GAGenome.h>
#include <ga/GA1DBinStrGenome.h>
#include <ga/GA1DArrayGenome.C>
#include <iostream>
#include <utility>

using namespace std;

namespace performance {

    class FileLogger {

    public:
        FileLogger(string folder, const string& fileMetaInfo);
        void saveGenerationPerformance(GAPopulation& population);
        void saveGenomeAsBest(GAGenome& genome);
        void saveTrialsPerformance(int trials, double performance[]);
        void close();

    private:
        void saveBestOfGeneration(int generation, const GAGenome& genome);
        string folder;
        string metainfo;
        ofstream file;
    };
}

#endif //SWARM_GEN_FILELOGGER_H
