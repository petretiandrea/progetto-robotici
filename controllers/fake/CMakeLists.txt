add_library(footbot_fake SHARED fake_controller.h)

target_link_libraries(footbot_fake
        argos3core_simulator
        argos3plugin_simulator_footbot
        argos3plugin_simulator_genericrobot)