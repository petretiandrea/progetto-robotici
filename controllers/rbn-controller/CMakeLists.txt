link_directories(${CMAKE_BINARY_DIR}/utility)

add_library(rbn_controller SHARED lua-wrapper/LuaBoolNetController.h lua-wrapper/LuaBoolNetController.cpp BooleanNetwork.h BooleanNetwork.cpp)

target_link_libraries(rbn_controller
        utility
        argos3core_simulator
        argos3plugin_simulator_footbot
        argos3plugin_simulator_genericrobot
        ${LUA_LIBRARIES})