link_directories(
        ${CMAKE_BUILD_DIR}/build/loop-functions/evolution
        ${CMAKE_BINARY_DIR}/controllers/rbn-controller
        ${CMAKE_BINARY_DIR}/utility
        ${CMAKE_BINARY_DIR}/common
)



if(GALIB_FOUND)
    set(DEPS ${GALIB_LIBRARIES}
            utility
            common
            rbn_controller
            evolution_loop
            argos3core_simulator)

    add_executable(swarm_test test_single.cpp)
    add_executable(swarm_viz visualize.cpp)
    add_executable(swarm_compare compare.cpp)

    target_link_libraries(swarm_test ${DEPS})
    target_link_libraries(swarm_viz ${DEPS})
    target_link_libraries(swarm_compare ${DEPS})

endif(GALIB_FOUND)
