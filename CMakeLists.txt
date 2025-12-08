cmake_minimum_required(VERSION 3.15)
project(AEC VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include Conan dependencies if present
if(EXISTS "${CMAKE_BINARY_DIR}/cmakedeps_macros.cmake")
    include("${CMAKE_BINARY_DIR}/cmakedeps_macros.cmake")
endif()
if(EXISTS "${CMAKE_BINARY_DIR}/GTestConfig.cmake")
    include("${CMAKE_BINARY_DIR}/GTestConfig.cmake")
endif()
if(EXISTS "${CMAKE_BINARY_DIR}/benchmark-config.cmake")
    include("${CMAKE_BINARY_DIR}/benchmark-config.cmake")
endif()

# Build options
option(AEC_BUILD_TESTS "Build tests" ON)
option(AEC_BUILD_BENCHMARKS "Build benchmarks" ON)
option(AEC_BUILD_EXAMPLES "Build examples" ON)

# Include directories
include_directories(include)

# Main library
add_library(aec
    src/aec.cpp
    src/fixed_point.cpp
    src/nlms_filter.cpp
)

target_include_directories(aec PUBLIC include)

# Tests
if(AEC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Benchmarks
if(AEC_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Examples
if(AEC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
install(TARGETS aec
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/aec
    DESTINATION include
)
