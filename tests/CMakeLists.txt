link_directories(
        ${CMAKE_BINARY_DIR}/controllers/rbn-controller
        ${CMAKE_BINARY_DIR}/utility
)

add_executable(swarm_gen main.cpp)

target_link_libraries(swarm_gen
        utility
        rbn_controller)