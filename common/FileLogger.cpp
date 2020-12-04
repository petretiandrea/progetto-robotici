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

// TODO: refactor duplicate code
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

// TODO: refactor duplicate code
void FileLogger::saveTrialsPerformance(int trials, double performance[], double robotCount[]) {
    ofstream trialsPerformance;
    trialsPerformance.open(folder + metainfo + ".csv");
    trialsPerformance << "trial;performance;robot_max_count" << endl;
    for(int i = 0; i < trials; i++) {
        trialsPerformance << i << ";" << performance[i] << ";" << robotCount[i] << endl;
    }
    trialsPerformance << flush;
    trialsPerformance.close();
}