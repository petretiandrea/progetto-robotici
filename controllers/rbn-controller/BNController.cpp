//
// Created by Andrea Petreti on 05/11/2020.
//

#include "BNController.h"
#include "FootbotUtils.h"
#include <constants.h>
#include <serializer/BooleanNetworkParser.h>

#define INPUT_NODE 12
#define OUTPUT_NODE 2

BNController::BNController() : wheels(nullptr), proximity(nullptr), motorGround(nullptr), booleanNetwork(nullptr) { }

BNController::~BNController() {
    delete booleanNetwork;
}

void BNController::Init(TConfigurationNode &t_node) {
    try {
        wheels = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
        proximity = GetSensor<CCI_FootBotProximitySensor>("footbot_proximity");
        motorGround = GetSensor<CCI_FootBotMotorGroundSensor>("footbot_motor_ground");
    } catch(CARGoSException& ex) {
        THROW_ARGOSEXCEPTION_NESTED("Error initializing sensors/actuators", ex);
    }

    booleanNetwork = new BooleanNetwork(
            constants::NODES,
            constants::INPUT_FOR_NODE,
            constants::BIAS,
            INPUT_NODE,
            OUTPUT_NODE,
            constants::RANDOM_SEED);

    string networkFile;
    GetNodeAttributeOrDefault(t_node, "network", networkFile, string(""));
    if(!networkFile.empty()) LoadFromFile(networkFile);
}

// TODO: improve this
void BNController::LoadFromFile(const string& filename){
    std::ifstream file(filename, ios::in);
    if(!file) { cerr << "Error opening network controller file" << endl; }

    vector<string> csv;
    string line, word;
    getline(file, line); // skip first line header
    while (getline(file, word, ';')) {
        csv.push_back(word);
    }
    auto genome = utility::trim(csv[1]);
    auto functionLength = pow(2, booleanNetwork->getInputForNode());

    cout << "Parsing boolean function from csv " << csv << endl;

    auto booleans = BooleanNetworkParser::booleanFunctionFromGenome(functionLength, genome);
    booleanNetwork->changeBooleanFunction(booleans);
}

void BNController::ControlStep() {
    auto proximityValues = footbotutils::readProximityValues(*proximity, 8);
    auto groundValues = footbotutils::readMotorGroundValues(*motorGround);

    auto proximityBooleans = footbotutils::sensorValuesToBooleans(proximityValues, constants::PROXIMITY_SIGHT);
    auto groundBooleans = footbotutils::sensorValuesToBooleans(groundValues, constants::MOTOR_GROUND_BLACK_THRESHOLD, true);

    // two for = more performance than: booleanNetwork->forceInputValues(proximityBooleans);
    for(int i = 0; i < proximityBooleans.size(); i++) {
        booleanNetwork->forceInputValue(i, proximityBooleans[i]);
    }

    for(int i = 0, offset = proximityBooleans.size(); i < groundBooleans.size(); i++) {
        booleanNetwork->forceInputValue(i + offset, groundBooleans[i]);
    }

    booleanNetwork->update();
    auto output = booleanNetwork->getOutputValues();
    moveRobotByBooleans(*wheels, output, constants::CONSTANT_SPEED_OUTPUT);
}

void BNController::moveRobotByBooleans(CCI_DifferentialSteeringActuator& wheels, const vector<bool>& output, float speed) {
    wheels.SetLinearVelocity(((float) utility::boolToInt(output[0])) * speed, ((float) utility::boolToInt(output[1])) * speed);
}

void BNController::Reset() {
    CCI_Controller::Reset();
    booleanNetwork->resetStates();
}

void BNController::Destroy() {

}

REGISTER_CONTROLLER(BNController, "bn_controller")
