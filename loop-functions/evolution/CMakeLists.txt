# TODO: link to controller
# link_directories(${CMAKE_BINARY_DIR}/controllers/footbot_nn)

add_library(evolution_loop SHARED evolution_loop.cpp Circle.h)

target_link_libraries(evolution_loop
        argos3core_simulator
        argos3plugin_simulator_dynamics2d
        argos3plugin_simulator_entities
        argos3plugin_simulator_footbot
        argos3plugin_simulator_genericrobot
        argos3plugin_simulator_media)
        # ${GALIB_LIBRARIES}) TODO: really?
