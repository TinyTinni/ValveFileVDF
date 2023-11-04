set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run")

add_executable(fuzzing main.cpp)
target_link_libraries(
    fuzzing
  PRIVATE
          -coverage
          -fsanitize=fuzzer)
target_compile_options(fuzzing PRIVATE -fsanitize=fuzzer)
add_test(NAME fuzzing_run COMMAND fuzzing -max_total_time=${FUZZ_RUNTIME} -timeout=${FUZZ_RUNTIME}) 