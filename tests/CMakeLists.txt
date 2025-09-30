# Tests CMakeLists.txt
find_package(GTest REQUIRED)

add_executable(aec_tests
    test_aec.cpp
    test_fixed_point.cpp
    test_nlms.cpp
)

target_link_libraries(aec_tests aec GTest::GTest GTest::Main)

add_test(NAME aec_tests COMMAND aec_tests)
