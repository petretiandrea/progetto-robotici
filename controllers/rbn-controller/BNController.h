//
// Created by Andrea Petreti on 05/11/2020.
//

#ifndef SWARM_GEN_BNCONTROLLER_H
#define SWARM_GEN_BNCONTROLLER_H

#include "BooleanNetwork.h"
#include <utility/Matrix.h>
#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_motor_ground_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>

using namespace argos;

class BNController : public CCI_Controller {

public:
    BNController();
    ~BNController() override;

    void Init(TConfigurationNode& t_node) override;
    void ControlStep() override;
    void Reset() override;
    void Destroy() override;

    BooleanNetwork& associatedNetwork() {  return *booleanNetwork; }

private:
    static void moveRobotByBooleans(CCI_DifferentialSteeringActuator& wheels, const vector<bool>& output, float speed);

private:
    CCI_DifferentialSteeringActuator* wheels;
    CCI_FootBotProximitySensor* proximity;
    CCI_FootBotMotorGroundSensor* motorGround;

    BooleanNetwork* booleanNetwork;
};

#endif //SWARM_GEN_BNCONTROLLER_H
