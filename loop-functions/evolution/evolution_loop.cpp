//
// Created by Andrea Petreti on 03/11/2020.
//
#include "evolution_loop.h"
#include <constants.h>

#define KEY_CIRCLE1 "circle1"
#define KEY_CIRCLE2 "circle2"
#define KEY_FOOTBOT_NUMBER "n_footbot"

EvolutionLoop::EvolutionLoop() :
    initialSpawnLocations(),
    randomGenerator(nullptr),
    currentTrial(0) {
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

    randomGenerator = CRandom::CreateRNG("argos");

    // create robots
    for(int i = 0; i < footbotNumber; i++) {
        auto bot = new CFootBotEntity("fb_" + std::to_string(i), "bn");
        bots.push_back(bot);
        controllers.push_back(&dynamic_cast<BNController&>(bot->GetControllableEntity().GetController()));
        AddEntity(*bot);
    }
}

/**
 * Generate a matrix of random locations for each trial
 * @param nTrials
 */
void EvolutionLoop::GenerateRandomSpawnLocation(int nTrials) {
    this->initialSpawnLocations = Matrix<SInitSetup>(nTrials, bots.size());

    for (int i = 0; i < nTrials; i++) {
        // compute all locations for trials
        auto locations = ComputeSpawnLocations(i);
        for(int j = 0; j < locations.size(); j++) {
            this->initialSpawnLocations.put(i, j, locations[j]);
        }
    }
}

/**
 * Spawn robot to right location from location matrix calculated previously.
 * @param nTrial
 */
void EvolutionLoop::PrepareForTrial(int nTrial) {
    this->currentTrial = nTrial;
}

/**
 * Compute a vector of random location for each bot
 * @param nTrial number of current trial
 * @return A vector of random location
 */
vector<EvolutionLoop::SInitSetup> EvolutionLoop::ComputeSpawnLocations(int nTrial) {
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
        } catch (std::logic_error& e) {
            std::cerr << "Error for bot: " << bot->GetId() << " on trial: " << nTrial << " reason: " << e.what() << std::endl;
        }
    }
    return botLocations;
}

/**
 * Generate a new random location checking collision. If collision exist it retry to generate a new location
 * for maxAttempts times.
 * @param maxAttempts number of max attempt.
 * @param botLocations current bot locations.
 * @return
 */
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

/**
 * Check a collision of point with a set of locations
 * @param position position to be verified
 * @param botLocations actual location of bots
 * @return
 */
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
    if (IsInsideCircles(CVector2(position.GetX(), position.GetY()))) return true;

    return false;
}

// TODO: call inside post experiment, at the end of experiment
double EvolutionLoop::CalculatePerformance() {
    CVector2 botPosition;
    double botCountForCircle[] = { 0, 0 };
    for(auto& bot : bots) {
        bot->GetEmbodiedEntity().GetOriginAnchor().Position.ProjectOntoXY(botPosition);
        if(blackCircles[0].containsPoint(botPosition)) botCountForCircle[0] += 1;
        if(blackCircles[1].containsPoint(botPosition)) botCountForCircle[1] += 1;
    }
    botCountForCircle[0] /= bots.size();
    botCountForCircle[1] /= bots.size();

    return (botCountForCircle[0] > botCountForCircle[1]) ? botCountForCircle[0] : botCountForCircle[1];
}

void EvolutionLoop::Reset() {
    /* Spawn robot to current trial location */
    for(int i = 0; i < this->initialSpawnLocations.getColumns(); i++) {
        auto location = initialSpawnLocations(currentTrial, i);
        auto bot = bots[i];

        if(!MoveEntity(bot->GetEmbodiedEntity(), location.Position, location.Orientation, false, true)) {
            //std::cerr << "Error for bot: " << bot->GetId() << " on trial: " << currentTrial << " reason: " << " cannot move entity " << std::endl;
        }
    }
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

void EvolutionLoop::ConfigureFromGenome(const GA1DBinaryStringGenome& genome) {
    auto functionLength = (int) pow(2, controllers.back()->associatedNetwork().getInputForNode());

    /* for loop = better performance than functional approach
     * auto genomeMatrix = fwd::apply(numbers(0, genome.size()), fwd::transform([genome](int i) { return (bool) genome.gene(i); }), fwd::split_every(8));
     * Matrix<bool> booleanFunctions = Matrix<bool>(genomeMatrix);
     * */
    Matrix<bool> booleanFunctions = Matrix<bool>(genome.size() / functionLength, functionLength);
    for(int i = 0; i < genome.size() / functionLength; i++) {
        for(int j = 0; j < functionLength; j++) {
            booleanFunctions.put(i, j, (bool) genome.gene(i * functionLength + j));
        }
    }

    for(auto& controller : controllers) {
        controller->associatedNetwork().changeBooleanFunction(booleanFunctions);
    }
}

void EvolutionLoop::dumpPosition() {
    CVector2 v;
    cout << "Pos " << endl;
    for(auto& bot : bots) {
        bot->GetEmbodiedEntity().GetOriginAnchor().Position.ProjectOntoXY(v);
        cout << v << endl;
    }

}

/* Register function loop to argos */
REGISTER_LOOP_FUNCTIONS(EvolutionLoop, "evolution_loop")