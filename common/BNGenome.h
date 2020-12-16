//
// Created by Andrea Petreti on 10/11/2020.
//

#ifndef SWARM_GEN_BNGENOME_H
#define SWARM_GEN_BNGENOME_H

#include <ga/GAGenome.h>
#include <ga/GA1DBinStrGenome.h>
#include <utility/NewRandom.h>
#include <argos3/core/simulator/simulator.h>
#include <loop-functions/evolution/evolution_loop.h>
#include <ostream>
#include <utility/Printable.h>

extern bool debug;

namespace bngenome {

    typedef struct {
        argos::CSimulator* simulator;
        EvolutionLoop* loop;
        int nTrials;
    } Experiment;

    GAGenome::Initializer genomeInitializer(float bias, newrandom::Random *rnd);
    float genomeEvaluator(GAGenome& genome);

    static function<void(GAGenome&)> functionInitializerCallback;
    static void initializer(GAGenome& genome) { functionInitializerCallback(genome); }


    class CustomEvalData : public GAEvalData, public Printable {
        public:
            double robotCount;
        public:
            CustomEvalData() : robotCount(0) {}
            explicit CustomEvalData(double robotCount) : robotCount(robotCount) {}
            ~CustomEvalData() override = default;

            GAEvalData *clone() const override {
                return new CustomEvalData(this->robotCount);
            }
            void copy(const GAEvalData &data) override {
                const auto& customData = dynamic_cast<const CustomEvalData*>(&data);
                this->robotCount = customData->robotCount;
            }

        ostream &print(ostream &os) override {
            os << this->robotCount;
            return os;
        }
    };
}

#endif //SWARM_GEN_BNGENOME_H
