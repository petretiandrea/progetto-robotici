//
// Created by Andrea Petreti on 03/11/2020.
//

#ifndef SWARM_GEN_FAKE_CONTROLLER_H
#define SWARM_GEN_FAKE_CONTROLLER_H

#include <argos3/core/control_interface/ci_controller.h>

using namespace argos;

class FakeController : public CCI_Controller {

public:
    FakeController();
    ~FakeController() override;

    void Init(TConfigurationNode& t_node) override;
};

FakeController::FakeController() = default;
FakeController::~FakeController() = default;

void FakeController::Init(TConfigurationNode &t_node) {}

REGISTER_CONTROLLER(FakeController, "fake_controller")

#endif //SWARM_GEN_FAKE_CONTROLLER_H
