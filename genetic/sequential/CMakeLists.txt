link_directories(
        ${CMAKE_BUILD_DIR}/build/loop-functions/evolution
        ${CMAKE_BINARY_DIR}/controllers/rbn-controller
        ${CMAKE_BINARY_DIR}/utility
)

if(GALIB_FOUND)
    add_executable(swarm_evolve sequential.cpp ../../common/BNGenome.h ../../common/BNGenome.cpp)
    target_link_libraries(swarm_evolve
            ${GALIB_LIBRARIES}
            utility
            common
            rbn_controller
            evolution_loop
            argos3core_simulator)
endif(GALIB_FOUND)
