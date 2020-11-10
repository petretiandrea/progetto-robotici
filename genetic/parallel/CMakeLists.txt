link_directories(
        ${CMAKE_BUILD_DIR}/build/loop-functions/evolution
        ${CMAKE_BINARY_DIR}/controllers/rbn-controller
        ${CMAKE_BINARY_DIR}/utility
)

if(GALIB_FOUND)
    add_executable(swarm_evolve_parallel parallel.cpp ParallelEvaluator.h SharedMemory.h ../BNGenome.h ../BNGenome.cpp)
    target_link_libraries(swarm_evolve_parallel
            rt
            ${GALIB_LIBRARIES}
            utility
            rbn_controller
            evolution_loop
            argos3core_simulator)
endif(GALIB_FOUND)
