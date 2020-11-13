link_directories(
        ${CMAKE_BUILD_DIR}/build/loop-functions/evolution
        ${CMAKE_BINARY_DIR}/controllers/rbn-controller
        ${CMAKE_BINARY_DIR}/utility
)

if(GALIB_FOUND)
    add_subdirectory(parallel)
    add_subdirectory(sequential)
endif(GALIB_FOUND)

