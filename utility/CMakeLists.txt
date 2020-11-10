include_directories(${CMAKE_SOURCE_DIR}/utility)

add_library(utility SHARED Matrix.h NewRandom.h Utility.h NewRandom.cpp)
