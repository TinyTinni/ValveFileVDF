cmake_minimum_required (VERSION 3.5.1)
project (vdf-Parser)

#set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set (EXECUTABLE_NAME "vdf-Parser-test")

set(CMAKE_INCLUDE_CURRENT_DIR ON)


#if (NOT Boost_LIBRARY_DIR)
#	message(FATAL_ERROR "Could not find Boost_LIBRARY_DIR. Please specify")
#endif()
link_directories(${Boost_LIBRARY_DIR})
find_package(Boost REQUIRED)

set(SSU_Sources "vdf_parser_test.cpp")

include_directories(${Boost_INCLUDE_DIR})
include_directories("../")

add_executable(${EXECUTABLE_NAME} ${SSU_Sources})

add_definitions("-DSOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")

if (WIN32)
    set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /bigobj" )
    add_definitions("-D_WIN32_WINNT=0x0501;WIN32_LEAN_AND_MEAN")
endif()


