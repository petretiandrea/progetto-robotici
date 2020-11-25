include_directories(${CMAKE_SOURCE_DIR}/common)

add_library(common SHARED FileLogger.h FileLogger.cpp BNGenome.h BNGenome.cpp)
