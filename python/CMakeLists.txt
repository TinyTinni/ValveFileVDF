cmake_minimum_required (VERSION 3.5.1)
project (vdf-Parser-Python)
set (TARGET_NAME "vdf")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PythonLibs)
include_directories (${PYTHON_INCLUDE_DIRS})

set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost REQUIRED COMPONENTS python3)

set(SSU_Sources "vdf_python_bindings.cpp")

include_directories(${Boost_INCLUDE_DIR})
link_directories(${PYTHON_LIBRARY})
link_directories(${BOOST_LIBRARYDIR})

add_library(${TARGET_NAME} SHARED "../vdf_parser.hpp" ${SSU_Sources})
target_link_libraries(${TARGET_NAME} ${Boost_LIBRARIES} )
target_link_libraries(${TARGET_NAME} ${PYTHON_LIBRARY} )
set_target_properties(${TARGET_NAME}   PROPERTIES SUFFIX ".pyd")

add_definitions("-DSOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")

if (WIN32)
    set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}" )
    add_definitions("-D_WIN32_WINNT=0x0501;WIN32_LEAN_AND_MEAN")
endif()


