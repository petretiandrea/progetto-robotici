//
// Created by Andrea Petreti on 26/11/2020.
//

#include <argos3/core/simulator/simulator.h>
#include <loop-functions/evolution/evolution_loop.h>
#include <utility/rapidcsv.h>
#include <unordered_set>
#include <vector>
#include <algorithm>

const string UNDER_TEST("performance/evolve_11-27_12-24_nodes50_k3_p100_g100_pM0.03_pC0.1_el5_bias0.21_prox0_trials3.csv");
const string OUTPUT_FILE("performance/compare_11-27_12-24_nodes50_k3_p100_g100_pM0.03_pC0.1_el5_bias0.21_prox0_trials3.csv");
const int BEST_GENOME = 30;
const int RUN_FOR_GENOME = 10;

struct PerformanceRecord {
    int generation;
    float score;
    string genome;
};

void distinct(std::vector<PerformanceRecord> &v)
{
    std::unordered_set<string> s;
    auto end = std::remove_if(v.begin(), v.end(), [&s](PerformanceRecord const &i) {
        return !s.insert(i.genome).second;
    });
    v.erase(end, v.end());
}

int main() {

    rapidcsv::Document document(UNDER_TEST, rapidcsv::LabelParams(0, -1), rapidcsv::SeparatorParams(';'));
    std::cout << "Reading " << document.GetRowCount() << " " << document.GetColumnCount() << endl;

    std::vector<PerformanceRecord> records;
    for(int i = 0; i < document.GetRowCount(); i++) {
        records.push_back(PerformanceRecord {
            .generation = document.GetCell<int>(0, i),
            .score = document.GetCell<float>(1, i),
            .genome = document.GetCell<string>(2, i)
        });
    }

    std::sort(records.rbegin(), records.rend(), [](const PerformanceRecord& r1, const PerformanceRecord& r2) {
        if(r1.score == r2.score) return r1.generation < r2.generation;
        return r1.score < r2.score;
    });

    distinct(records);

    std::vector<PerformanceRecord> best(records.begin(), records.begin() + BEST_GENOME);
    for(int i = 0; i < best.size(); i++) {
        cout << best[i].generation << " " << best[i].score << " " << best[i].genome <<  endl;
    }


    // evaluate the best
    double performanceBest[BEST_GENOME][RUN_FOR_GENOME];
    static argos::CSimulator& simulator = argos::CSimulator::GetInstance();

    simulator.SetExperimentFileName("tests/compare.argos");
    simulator.LoadExperiment();

    static EvolutionLoop* loop = dynamic_cast<EvolutionLoop*>(&simulator.GetLoopFunctions());

    loop->GenerateRandomSpawnLocation(RUN_FOR_GENOME);

    ofstream compareFile;
    compareFile.open(OUTPUT_FILE);
    compareFile << "genomeIndex;runIndex;runScore;geneticScore" << flush << endl;
    for(int i = 0; i < best.size(); i++) {
        loop->ConfigureFromGenome(best[i].genome);

        for(int j = 0; j < RUN_FOR_GENOME; j++) {
            loop->PrepareForTrial(j);
            simulator.Reset();
            simulator.Reset();
            simulator.Execute();

            auto score = loop->CalculatePerformance();
            cout << "Performance of " << i << " is: " <<  score << endl;
            performanceBest[i][j] = score;
            compareFile << i << ";" << j << ";" << score << ";" << best[i].score << endl;
        }
    }
    compareFile.close();
    return 0;
}