//
// Created by Andrea Petreti on 23/11/2020.
//

#include "FileLogger.h"

using namespace performance;

FileLogger::FileLogger(string folder, const string& fileMetaInfo) : folder(std::move(folder)), metainfo(fileMetaInfo) {
    file.open(this->folder + fileMetaInfo + ".csv");
    file << "generation;score;genome;bn" << endl << flush;
}

void FileLogger::saveGenerationPerformance(GAPopulation& population) {
    for(int i = 0; i < population.size(); i++) {
        file << population.geneticAlgorithm()->generation() << ";"
             << population.individual(i).score() << ";"
             << population.individual(i) << ";"
             << endl << flush;
    }
    // TODO: this provide the best or the best for current population?
    saveBestOfGeneration(population.geneticAlgorithm()->generation(), population.best(0));
}

void FileLogger::saveBestOfGeneration(int generation, const GAGenome &genome) {
    ofstream bestFile;
    bestFile.open(folder + metainfo + "_best_gen_" + to_string(generation) + ".csv");
    bestFile << "score;genome;bn" << endl;
    bestFile << genome.score() << ";" << genome << endl << flush;
    bestFile.close();
}

void FileLogger::close() {
    file.flush();
    file.close();
}

void FileLogger::saveGenomeAsBest(GAGenome& genome) {
    ofstream bestFile;
    bestFile.open(folder + metainfo + "_best_all.csv");
    bestFile << "score;genome;bn" << endl;
    cout << genome.score() << endl;
    bestFile << genome.score() << ";" << genome << endl << flush;
    bestFile.close();
}

void FileLogger::saveTrialsPerformance(int trials, double performance[]) {
    ofstream trialsPerformance;
    trialsPerformance.open(folder + metainfo + ".csv");
    trialsPerformance << "trial;performance" << endl;
    for(int i = 0; i < trials; i++) {
        trialsPerformance << i << ";" << performance[i] << endl;
    }
    trialsPerformance << flush;
    trialsPerformance.close();
}