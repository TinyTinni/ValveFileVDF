set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run")

add_executable(fuzzing main.cpp)
target_compile_features(fuzzing PUBLIC cxx_std_17)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_link_libraries(fuzzing PUBLIC -coverage -fsanitize=fuzzer,address,undefined)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	target_compile_options(fuzzing PRIVATE /fsanitize=fuzzer /fsanitize=address)
endif()

target_link_libraries(fuzzing PRIVATE ValveFileVDF)

add_test(NAME fuzzing_run COMMAND fuzzing -max_total_time=${FUZZ_RUNTIME} -timeout=${FUZZ_RUNTIME})