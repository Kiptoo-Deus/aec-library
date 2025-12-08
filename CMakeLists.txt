
cmake_minimum_required(VERSION 3.15)
project(aec-library LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(AEC_SRC
    src/aec.cpp
    src/fixed_point.cpp
    src/nlms_filter.cpp
    src/aec_jni.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

if(IOS)
    message(STATUS "Building for iOS")
    add_library(aec SHARED ${AEC_SRC})
    set_target_properties(aec PROPERTIES
        OUTPUT_NAME "aec"
        FRAMEWORK TRUE
        PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/aec/aec.hpp"
    )
    target_include_directories(aec PRIVATE ${CMAKE_SOURCE_DIR}/include)
elseif(ANDROID)
    message(STATUS "Building for Android")
    add_library(aec SHARED ${AEC_SRC})
    set_target_properties(aec PROPERTIES
        OUTPUT_NAME "aec"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    target_include_directories(aec PRIVATE ${CMAKE_SOURCE_DIR}/include)
else()
    add_library(aec STATIC ${AEC_SRC})
    set_target_properties(aec PROPERTIES
        OUTPUT_NAME "aec"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    target_include_directories(aec PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()

# Link dependencies (if any from Conan)
if(TARGET CONAN_PKG::gtest)
    target_link_libraries(aec PRIVATE CONAN_PKG::gtest)
endif()

# Examples
add_executable(aec_example examples/basic_usage.cpp)
target_link_libraries(aec_example PRIVATE aec)

# Benchmarks
add_executable(aec_benchmark benchmarks/aec_benchmark.cpp)
target_link_libraries(aec_benchmark PRIVATE aec)

# Tests
enable_testing()
add_executable(aec_test tests/test_aec.cpp tests/test_fixed_point.cpp tests/test_nlms.cpp)
target_link_libraries(aec_test PRIVATE aec)
add_test(NAME aec_test COMMAND aec_test)
