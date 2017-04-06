cmake_minimum_required (VERSION 3.5.1)
project (vdf-Python)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

	
find_package(Git REQUIRED)
option(DOWNLOAD_PYBIND11 "Download PyBind11. (Requires Git)" ${Git_FOUND})

set(PYBIND11_PATH "${CMAKE_CURRENT_SOURCE_DIR}/pybind11" CACHE FILEPATH "Path to pybind11")
if (${DOWNLOAD_PYBIND11})	
	#include(ExternalProject)
    set (_GIT_CMD "clone")
    if (EXISTS ${PYBIND11_PATH})
        message(STATUS "pull pybind11")
        execute_process(COMMAND ${GIT_EXECUTABLE} pull  
                    WORKING_DIRECTORY "${PYBIND11_PATH}")
    else()
        message(STATUS "clone pybind11")
        execute_process(COMMAND ${GIT_EXECUTABLE} ${_GIT_CMD} https://github.com/pybind/pybind11.git ${PYBIND11_PATH}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	endif()
endif()

find_package(PythonLibs REQUIRED)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set(PYTHON_MODULE_EXTENSION ".pyd")

if (NOT EXISTS ${PYBIND11_PATH}/CMakeLists.txt)
    message(FATAL_ERROR "Could not found pybind11.\n Please specify PYBIND11_PATH or check DOWNLOAD_PYBIND11")
endif()

add_subdirectory(${PYBIND11_PATH})
pybind11_add_module(vdf "../vdf_parser.hpp;vdf_python_bindings.cpp")


add_definitions("-D_WIN32_WINNT=0x0501;WIN32_LEAN_AND_MEAN")