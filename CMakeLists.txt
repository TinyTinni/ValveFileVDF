cmake_minimum_required (VERSION 3.6)
project (vdf-Parser)

include(CTest)
add_subdirectory(./tests)

## add fuzzing, if supported

function(check_libfuzzer_support var_name)
  set(LibFuzzerTestSource
      "
#include <cstdint>

extern \"C\" int LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
  return 0;
}
    ")

  include(CheckCXXSourceCompiles)

  set(CMAKE_REQUIRED_FLAGS "-fsanitize=fuzzer")
  set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=fuzzer")
  check_cxx_source_compiles("${LibFuzzerTestSource}" ${var_name})

endfunction()

check_libfuzzer_support(ENABLE_FUZZING)
if (${ENABLE_FUZZING})
    add_subdirectory(fuzzing)
endif()