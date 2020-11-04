//
// Created by Andrea Petreti on 03/11/2020.
//
#include "evolution_loop.h"
#include <constants.h>

using namespace argos;

#define KEY_CIRCLE1 "circle1"
#define KEY_CIRCLE2 "circle2"
#define KEY_FOOTBOT_NUMBER "n_footbot"

EvolutionLoop::EvolutionLoop() :
    currentTrial(0),
    randomGenerator(nullptr) {
}

EvolutionLoop::~EvolutionLoop() = default;

void EvolutionLoop::Init(TConfigurationNode &t_tree) {
    CLoopFunctions::Init(t_tree);

    // add circles
    CVector3 tmpCircle;
    GetNodeAttributeOrDefault(t_tree, KEY_CIRCLE1, tmpCircle, constants::CIRCLE1);
    blackCircles.push_back(Circle { tmpCircle.GetX(), tmpCircle.GetY(), tmpCircle.GetZ() });
    GetNodeAttributeOrDefault(t_tree, KEY_CIRCLE2, tmpCircle, constants::CIRCLE2);
    blackCircles.push_back(Circle { tmpCircle.GetX(), tmpCircle.GetY(), tmpCircle.GetZ() });

    int footbotNumber;
    GetNodeAttributeOrDefault(t_tree, KEY_FOOTBOT_NUMBER, footbotNumber, constants::N_FOOTBOT);

    /*
     * TODO: when externally is called reset on simulator, the RNG of argos is reset, this spawn robots always at the same position.
     * Actually workaround is create another generator using the same seed of argos
     */
    CRandom::CreateCategory("myrand", CRandom::GetSeedOf("argos"));
    randomGenerator = CRandom::CreateRNG("myrand");

    // create robots
    for(int i = 0; i < footbotNumber; i++) {
        auto bot = new CFootBotEntity("fb_" + std::to_string(i), "fake");
        bots.push_back(bot);
        controllers.push_back(&dynamic_cast<FakeController&>(bot->GetControllableEntity().GetController()));
        AddEntity(*bot);
    }
}

void EvolutionLoop::PrepareForTrial(int nTrial) {
    // spawn bot and generate position
    CRadians orientation;
    std::vector<SInitSetup> botLocations;

    for(auto& bot : bots) {
        try {
            SInitSetup spawnLocation;
            orientation = randomGenerator->Uniform(CRadians::UNSIGNED_RANGE);
            spawnLocation.Orientation.FromEulerAngles(
                    orientation,        // rotation around Z
                    CRadians::ZERO,     // rotation around Y
                    CRadians::ZERO      // rotation around X
            );

            CVector3 position = GenerateLocationWithoutCollision(constants::MAX_ATTEMPTS_LOCATION_SPAWN, botLocations);
            spawnLocation.Position.Set(position.GetX(), position.GetY(), position.GetZ());
            botLocations.push_back(spawnLocation);
            std::cout << spawnLocation.Position.GetX() << "  " << spawnLocation.Position.GetY() << std::endl;
            if(!MoveEntity(bot->GetEmbodiedEntity(), spawnLocation.Position, spawnLocation.Orientation, false)) {
                std::cerr << "Error for bot: " << bot->GetId() << " on trial: " << nTrial << " reason: " << " cannot move entity " << std::endl;
            }
        } catch (std::logic_error& e) {
            std::cerr << "Error for bot: " << bot->GetId() << " on trial: " << nTrial << " reason: " << e.what() << std::endl;
        }
    }
}

CVector3 EvolutionLoop::GenerateLocationWithoutCollision(int maxAttempts, const std::vector<SInitSetup>& botLocations) {
    bool collision = true;
    CVector3 position;
    int currentAttempt;

    double rangeSize = (constants::ARENA_SIDE_SIZE - 2 * constants::FOOTBOT_RADIUS) / 2;
    CRange<Real> rangeArena(-rangeSize, rangeSize);
    for(currentAttempt = 0; collision && currentAttempt < maxAttempts; currentAttempt++) {
        position.SetZ(0);
        position.SetX(randomGenerator->Uniform(rangeArena));
        position.SetY(randomGenerator->Uniform(rangeArena));
        collision = CheckCollision(position, botLocations);
    }
    if(currentAttempt >= maxAttempts) {
        throw std::logic_error("Total attempts reached for generate random location.");
    } else {
        return position;
    }
}

bool EvolutionLoop::CheckCollision(CVector3& position, const std::vector<SInitSetup>& botLocations) {
    // check collision with robots
    for(const auto& botLocation : botLocations) {
        //(r1+r2)^2 >= (x2-x1)^2+(y2-y1)^2
        double xDiff = position.GetX() - botLocation.Position.GetX();
        double yDiff = position.GetY() - botLocation.Position.GetY();
        double centerDiff = pow(xDiff, 2) + pow(yDiff, 2);
        if(pow(2 * constants::FOOTBOT_RADIUS, 2) >= centerDiff) return true;
    }

    // check collision over circles
    // TODO: check this collision? or spawn robot anywhere?
    // if (IsInsideCircles(CVector2(position.GetX(), position.GetY()))) return true;

    return false;
}


void EvolutionLoop::PreStep() {
    CLoopFunctions::PreStep();
}

void EvolutionLoop::PostStep() {
    CLoopFunctions::PostStep();
}

// TODO: call inside post experiment, at the end of experiment
double EvolutionLoop::CalculatePerformance() {
    CVector2 botPosition;
    double botCountForCircle[] = { 0, 0 };
    for(int i = 0; i < bots.size(); i++) {
        bots[i]->GetEmbodiedEntity().GetOriginAnchor().Position.ProjectOntoXY(botPosition);
        if(blackCircles[0].containsPoint(botPosition)) botCountForCircle[0] += 1;
        if(blackCircles[1].containsPoint(botPosition)) botCountForCircle[1] += 1;
    }
    botCountForCircle[0] /= bots.size();
    botCountForCircle[1] /= bots.size();

    return (botCountForCircle[0] > botCountForCircle[1]) ? botCountForCircle[0] : botCountForCircle[1];
}

void EvolutionLoop::Reset() {
    CLoopFunctions::Reset();
}

CColor EvolutionLoop::GetFloorColor(const CVector2 &c_pos_on_floor) {
    if (IsInsideCircles(c_pos_on_floor)) {
        return CColor::BLACK;
    }
    return CLoopFunctions::GetFloorColor(c_pos_on_floor);
}

bool EvolutionLoop::IsInsideCircles(const CVector2& point) {
    return std::any_of(blackCircles.begin(), blackCircles.end(), [point](Circle& circle) { return circle.containsPoint(point); });
}

/* Register function loop to argos */
REGISTER_LOOP_FUNCTIONS(EvolutionLoop, "evolution_loop")