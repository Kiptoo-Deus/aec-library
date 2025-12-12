# Examples CMakeLists.txt
add_executable(basic_usage
    basic_usage.cpp
)

target_link_libraries(basic_usage aec)

add_executable(wav_aec
    wav_aec.cpp
)
target_link_libraries(wav_aec aec)

install(TARGETS wav_aec DESTINATION bin)
