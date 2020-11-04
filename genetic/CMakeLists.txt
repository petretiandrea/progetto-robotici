link_directories(
        ${CMAKE_BUILD_DIR}/build/loop-functions/evolution
        ${CMAKE_BINARY_DIR}/controllers/fake
)

if(GALIB_FOUND)
    add_executable(swarm_evolve main.cpp)
    target_link_libraries(swarm_evolve
            ${GALIB_LIBRARIES}
            footbot_fake
            evolution_loop
            argos3core_simulator)
endif(GALIB_FOUND)

