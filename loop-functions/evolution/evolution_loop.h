//
// Created by Andrea Petreti on 02/11/2020.
//

#ifndef SWARM_GEN_EVOLUTION_LOOP_H
#define SWARM_GEN_EVOLUTION_LOOP_H

/* ARGoS headers */
#include <utility/Matrix.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <controllers/rbn-controller/BNController.h>
#include <ga/GA1DBinStrGenome.h>
#include "utility/Circle.h"

using namespace argos;
using namespace std;

class EvolutionLoop : public CLoopFunctions {

public:
    void dumpPosition();
    EvolutionLoop();
    ~EvolutionLoop() override;

    void Init(TConfigurationNode &t_tree) override;

    /* Allow to choose what color render, useful to draw floor shape */
    CColor GetFloorColor(const CVector2 &c_pos_on_floor) override;

    /**
     * Calculate all random location for x trials
     * @param nTrials
     */
    void GenerateRandomSpawnLocation(int nTrials);

    /**
     * Setup the environment for start the nth trial. It spawn footbot at random location (previously generated)
     * @param nTrial
     */
    void PrepareForTrial(int nTrial);

    void ConfigureFromGenome(const GA1DBinaryStringGenome& genome);

    double CalculatePerformance();

    void Reset() override;

private:
    struct SInitSetup {
        CVector3 Position;
        CQuaternion Orientation;
    };

    Matrix<SInitSetup> initialSpawnLocations;
    CRandom::CRNG* randomGenerator;
    vector<Circle> blackCircles;
    vector<CFootBotEntity*> bots;
    vector<BNController*> controllers;
    int currentTrial;

private:
    bool CheckCollision(CVector3& position, const std::vector<SInitSetup>& botLocations);
    vector<SInitSetup> ComputeSpawnLocations(int nTrial);
    CVector3 GenerateLocationWithoutCollision(int maxAttempts, const std::vector<SInitSetup>& botLocations);
    bool IsInsideCircles(const CVector2& point);
};

#endif //SWARM_GEN_EVOLUTION_LOOP_H