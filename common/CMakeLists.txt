include_directories(${CMAKE_SOURCE_DIR}/common)

add_library(common SHARED FileLogger.h FileLogger.cpp BNGenome.h BNGenome.cpp Experiment.h)
target_link_libraries(common ${GALIB_LIBRARIES})