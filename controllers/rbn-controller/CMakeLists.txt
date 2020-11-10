link_directories(
        ${CMAKE_BINARY_DIR}/utility
        ${CMAKE_BINARY_DIR}/thirdparty/fplus/include
)

add_library(rbn_controller SHARED BooleanNetwork.h BooleanNetwork.cpp BNController.h BNController.cpp FootbotUtils.h)
add_dependencies(rbn_controller functional_plus)

target_link_libraries(rbn_controller
        utility
        argos3core_simulator
        argos3plugin_simulator_footbot
        argos3plugin_simulator_genericrobot
        ${LUA_LIBRARIES})