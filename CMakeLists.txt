cmake_minimum_required (VERSION 3.6)
project (vdf-Parser)

add_library(ValveFileVDF INTERFACE)
target_include_directories(ValveFileVDF INTERFACE "include")

include(CTest)
add_subdirectory(./tests)

## add fuzzing, if supported

set(ENABLE_FUZZING OFF)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC" AND NOT APPLE)
	set(ENABLE_FUZZING ON)
endif()

if (${ENABLE_FUZZING})
    add_subdirectory(fuzzing)
endif()
