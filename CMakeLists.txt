cmake_minimum_required (VERSION 3.6)
project (vdf-Parser)

include(CTest)
add_subdirectory(./tests)

## add fuzzing, if supported

set( ENABLE_FUZZING OFF)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	set(ENABLE_FUZZING ON)
endif

if (${ENABLE_FUZZING})
    add_subdirectory(fuzzing)
endif()