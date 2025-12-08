# Benchmarks CMakeLists.txt

add_executable(aec_benchmark
    aec_benchmark.cpp
)

target_link_libraries(aec_benchmark aec benchmark::benchmark)
