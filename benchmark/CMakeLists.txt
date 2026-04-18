
add_executable(benchmark_vdf main.cpp)

include (../cmake/get_cpm.cmake)
CPMAddPackage(NAME benchmark
               GITHUB_REPOSITORY google/benchmark
               GIT_TAG v1.9.5
               OPTIONS "BENCHMARK_ENABLE_TESTING OFF")

target_link_libraries(benchmark_vdf PRIVATE ValveFileVDF::ValveFileVDF benchmark::benchmark)
target_compile_features(benchmark_vdf PUBLIC cxx_std_20)
