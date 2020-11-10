cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(swarm_gen)

set(CMAKE_CXX_STANDARD 14)

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
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS} ${GSL_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${FPLUS_INCLUDE_DIR})

# Set ARGoS link dir
link_directories(${ARGOS_LIBRARY_DIRS})

include(ExternalProject)
ExternalProject_Add(functional_plus
        GIT_REPOSITORY https://github.com/Dobiasd/FunctionalPlus.git
        GIT_TAG v0.2.10-p0

        SOURCE_DIR "${CMAKE_BINARY_DIR}/thirdparty/fplus"

        CONFIGURE_COMMAND ""
        INSTALL_COMMAND ""

        LOG_DOWNLOAD ON
        LOG_BUILD ON
        )

set(FPLUS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/thirdparty/fplus/include)
include_directories(${FPLUS_INCLUDE_DIR})

# -- add subdir for each module
add_subdirectory(utility)
add_subdirectory(loop-functions)
add_subdirectory(controllers)
add_subdirectory(genetic)
add_subdirectory(tests)