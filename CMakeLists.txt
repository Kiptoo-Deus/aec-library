
cmake_minimum_required(VERSION 3.15)
project(aec-library LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(AEC_SRC
    src/aec.cpp
    src/fixed_point.cpp
    src/nlms_filter.cpp
    src/double_talk_detector.cpp
    src/webrtc_adapter.cpp
)

# Optionally include JNI wrapper only if JNI is available or building for Android
if (ANDROID)
    list(APPEND AEC_SRC src/aec_jni.cpp)
    message(STATUS "Including JNI wrapper in Android build")
endif()

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

add_executable(aec_webrtc_demo examples/webrtc_aec_demo.cpp)
target_link_libraries(aec_webrtc_demo PRIVATE aec)

# Small examples
add_executable(wav_aec examples/wav_aec.cpp)
target_link_libraries(wav_aec PRIVATE aec)
install(TARGETS wav_aec DESTINATION bin)

option(ENABLE_BENCHMARKS "Enable building benchmarks" OFF)
if (ENABLE_BENCHMARKS)
    find_package(benchmark QUIET)
    if (benchmark_FOUND)
        add_executable(aec_benchmark benchmarks/aec_benchmark.cpp)
        target_link_libraries(aec_benchmark PRIVATE aec benchmark::benchmark)
    else()
        message(WARNING "benchmark not found; benchmarks disabled. Install libbenchmark to enable them.")
    endif()
endif()

option(ENABLE_UNIT_TESTS "Enable building unit tests" OFF)
if (ENABLE_UNIT_TESTS)
    find_package(GTest QUIET)
    if (NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG main
        )
        FetchContent_MakeAvailable(googletest)
        set(GTest_FOUND TRUE)
    endif()

    if (GTest_FOUND)
        enable_testing()
            add_executable(aec_test tests/test_aec.cpp tests/test_fixed_point.cpp tests/test_nlms.cpp tests/test_double_talk.cpp tests/test_multichannel.cpp tests/test_wav_aec.cpp tests/test_webrtc_adapter.cpp)
        target_link_libraries(aec_test PRIVATE aec GTest::gtest_main)
        include(GoogleTest)
        gtest_discover_tests(aec_test)
    else()
        message(WARNING "GTest not available; unit tests disabled.")
    endif()
endif()
