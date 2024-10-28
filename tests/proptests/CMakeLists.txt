include (../../cmake/get_cpm.cmake)

CPMAddPackage(NAME rapidcheck
    GITHUB_REPOSITORY "emil-e/rapidcheck"
    GIT_TAG "ff6af6fc683159deb51c543b065eba14dfcf329b"
    OPTIONS
    "RC_ENABLE_DOCTEST OFF"
    "CATCH_BUILD_TESTING OFF"
    "CATCH_INSTALL_DOCS OFF"
)

get_property(rapidcheck_include_dirs TARGET rapidcheck PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
set_property(TARGET rapidcheck PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${rapidcheck_include_dirs}")

add_executable(rapidcheck_tests "main_rapidcheck.cpp")
set_property(TARGET tests PROPERTY COMPILE_WARNING_AS_ERROR ON)
target_compile_features(rapidcheck_tests PUBLIC cxx_std_20)
target_link_libraries(rapidcheck_tests PRIVATE ValveFileVDF rapidcheck)

target_compile_options(rapidcheck_tests PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -Wconversion -pedantic-errors -Wsign-conversion>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_link_libraries(rapidcheck_tests PUBLIC -fsanitize=address,undefined)
endif()


add_test(NAME vdf_rapidcheck_tests COMMAND rapidcheck_tests)

