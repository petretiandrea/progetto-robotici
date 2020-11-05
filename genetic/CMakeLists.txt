link_directories(
        ${CMAKE_BUILD_DIR}/build/loop-functions/evolution
        ${CMAKE_BINARY_DIR}/controllers/fake
        ${CMAKE_BINARY_DIR}/controllers/rbn-controller
)

if(GALIB_FOUND)
    add_executable(swarm_evolve main.cpp)
    target_link_libraries(swarm_evolve
            ${GALIB_LIBRARIES}
            footbot_fake
            rbn_controller
            evolution_loop
            argos3core_simulator)
endif(GALIB_FOUND)

