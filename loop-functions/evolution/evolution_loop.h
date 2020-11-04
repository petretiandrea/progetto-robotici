//
// Created by Andrea Petreti on 02/11/2020.
//

#ifndef SWARM_GEN_EVOLUTION_LOOP_H
#define SWARM_GEN_EVOLUTION_LOOP_H

/* ARGoS headers */
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <controllers/fake/fake_controller.h>
#include "Circle.h"

using namespace argos;

class EvolutionLoop : public CLoopFunctions {

public:
    EvolutionLoop();
    ~EvolutionLoop() override;

    void Init(TConfigurationNode &t_tree) override;
    void PreStep() override;
    void PostStep() override;

    /* Allow to choose what color render, useful to draw floor shape */
    CColor GetFloorColor(const CVector2 &c_pos_on_floor) override;

    /**
     * Setup the environment for start the nth trial. It spawn footbot at random location
     * @param nTrial
     */
    void PrepareForTrial(int nTrial);

    void Reset() override;

private:
    struct SInitSetup {
        CVector3 Position;
        CQuaternion Orientation;
    };

    unsigned long currentTrial;
    CRandom::CRNG* randomGenerator;
    std::vector<Circle> blackCircles;
    std::vector<CFootBotEntity*> bots;
    std::vector<FakeController*> controllers;

private:
    double CalculatePerformance();
    bool CheckCollision(CVector3& position, const std::vector<SInitSetup>& botLocations);
    CVector3 GenerateLocationWithoutCollision(int maxAttempts, const std::vector<SInitSetup>& botLocations);
    bool IsInsideCircles(const CVector2& point);
};

#endif //SWARM_GEN_EVOLUTION_LOOP_H