cmake_minimum_required (VERSION 3.11.2018)
project (vdf-Python)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Git REQUIRED)
find_package(Python COMPONENTS Interpreter Development)

option(DOWNLOAD_PYBIND11 "Download PyBind11. (Requires Git)" ${Git_FOUND})

set(PYBIND11_PATH "${CMAKE_BINARY_DIR}/pybind11" CACHE FILEPATH "Path to pybind11")
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

if (NOT EXISTS ${PYBIND11_PATH}/CMakeLists.txt)
    message(FATAL_ERROR "Could not found pybind11.\n Please specify PYBIND11_PATH or check DOWNLOAD_PYBIND11")
endif()

add_subdirectory(${PYBIND11_PATH})
pybind11_add_module(vdf "../vdf_parser.hpp;vdf_python_bindings.cpp;Readme.md")
target_link_libraries(vdf PRIVATE Python::Python ValveFileVDF)


## add tests
include(CTest)
if (BUILD_TESTING)
    add_test(NAME vdf_python_test COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test.py WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../tests/testdata")
    set(ppath $ENV{PYTHONPATH})
    LIST(APPEND ppath  "$<TARGET_FILE_DIR:vdf>")
    set_tests_properties(vdf_python_test PROPERTIES ENVIRONMENT "PYTHONPATH=${ppath}" DEPENDS vdf)
endif()
