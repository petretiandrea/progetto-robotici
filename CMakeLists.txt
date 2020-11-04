cmake_minimum_required(VERSION 2.8.12)
project(swarm_gen)

# Set the path where CMake will find additional scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Find the ARGoS package
find_package(PkgConfig)
pkg_check_modules(ARGOS REQUIRED argos3_simulator)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)

# Check whether all the necessary libs have been installed to compile the
# code that depends on Qt and OpenGL
include(ARGoSCheckQTOpenGL)

# Look for the (optional) galib library
find_package(GALIB)
if(GALIB_FOUND)
    include_directories(${GALIB_INCLUDE_DIRS})
endif(GALIB_FOUND)

# Find the GSL package
find_package(GSL REQUIRED)

# Find Lua
find_package(Lua53 REQUIRED)

# Set ARGoS include dir
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS} ${GSL_INCLUDE_DIR} ${LUA_INCLUDE_DIR})

# Set ARGoS link dir
link_directories(${ARGOS_LIBRARY_DIRS})

# -- add subdir for each module
add_subdirectory(loop-functions)
add_subdirectory(controllers)
add_subdirectory(genetic)

link_directories(
        ${CMAKE_BUILD_DIR}/build/loop-functions/evolution
)

add_executable(swarm_gen main.cpp controllers/fake/fake_controller.h genetic/main.cpp)

target_link_libraries(swarm_gen
        ${GALIB_LIBRARIES}
        evolution_loop
        footbot_fake
        argos3core_simulator
        argos3plugin_simulator_dynamics2d
        argos3plugin_simulator_entities
        argos3plugin_simulator_footbot
        argos3plugin_simulator_genericrobot
        argos3plugin_simulator_media)