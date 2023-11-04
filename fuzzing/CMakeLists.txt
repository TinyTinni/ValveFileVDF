set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run")

set (CMAKE_CXX_STANDARD 17)
add_executable(fuzzing main.cpp)
target_link_libraries(fuzzing PRIVATE -coverage -fsanitize=fuzzer)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	target_compile_options(fuzzing PRIVATE -fsanitize=fuzzer)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	target_compile_options(fuzzing PRIVATE /fsanitize=fuzzer)
endif()

add_test(NAME fuzzing_run COMMAND fuzzing -max_total_time=${FUZZ_RUNTIME} -timeout=${FUZZ_RUNTIME}) 