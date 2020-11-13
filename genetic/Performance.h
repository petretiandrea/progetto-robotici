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
#include <utility>

using namespace std;

namespace performance {

    class PerformanceLog {

    public:
        PerformanceLog(string folder, const string& fileMetaInfo);
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

    PerformanceLog::PerformanceLog(string folder, const string& fileMetaInfo) : folder(std::move(folder)), metainfo(fileMetaInfo) {
        file.open(this->folder + fileMetaInfo + ".csv");
        file << "generation;score;genome;bn" << endl << flush;
    }

    void PerformanceLog::saveGenerationPerformance(GAPopulation& population) {
        for(int i = 0; i < population.size(); i++) {
            file << population.geneticAlgorithm()->generation() << ";"
                << population.individual(i).score() << ";"
                << population.individual(i) << ";"
                << endl << flush;
        }
        // TODO: this provide the best or the best for current population?
        saveBestOfGeneration(population.geneticAlgorithm()->generation(), population.best(0));
    }

    void PerformanceLog::saveBestOfGeneration(int generation, const GAGenome &genome) {
        ofstream bestFile;
        bestFile.open(folder + metainfo + "_best_gen_" + to_string(generation) + ".csv");
        bestFile << "score;genome;bn" << endl;
        bestFile << genome.score() << ";" << genome << endl << flush;
        bestFile.close();
    }

    void PerformanceLog::close() {
        file.flush();
        file.close();
    }

    void PerformanceLog::saveGenomeAsBest(GAGenome& genome) {
        ofstream bestFile;
        bestFile.open(folder + metainfo + "_best_all.csv");
        bestFile << "score;genome;bn" << endl;
        cout << genome.score() << endl;
        bestFile << genome.score() << ";" << genome << endl << flush;
        bestFile.close();
    }

    void PerformanceLog::saveTrialsPerformance(int trials, double performance[]) {
        ofstream trialsPerformance;
        trialsPerformance.open(folder + metainfo + ".csv");
        trialsPerformance << "trial;performance" << endl;
        for(int i = 0; i < trials; i++) {
            trialsPerformance << i << ";" << performance[i] << endl;
        }
        trialsPerformance << flush;
        trialsPerformance.close();
    }

}

#endif //SWARM_GEN_PERFORMANCE_H
