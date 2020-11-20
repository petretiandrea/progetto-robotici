link_directories(
        ${CMAKE_BINARY_DIR}/controllers/rbn-controller
        ${CMAKE_BINARY_DIR}/utility
        ${CMAKE_BINARY_DIR}/thirdparty/fplus/include
)

add_library(evolution_loop SHARED evolution_loop.h evolution_loop.cpp)
add_dependencies(evolution_loop functional_plus)

target_link_libraries(evolution_loop
        utility
        rbn_controller
        argos3core_simulator
        argos3plugin_simulator_dynamics2d
        argos3plugin_simulator_entities
        argos3plugin_simulator_footbot
        argos3plugin_simulator_genericrobot
        argos3plugin_simulator_media)
        # ${GALIB_LIBRARIES}) TODO: really?
