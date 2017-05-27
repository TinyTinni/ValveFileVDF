cmake_minimum_required (VERSION 2.8.7)
project (vdf-Parser)

#set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    catch
    PREFIX ${CMAKE_BINARY_DIR}/catch
    GIT_REPOSITORY https://github.com/philsquared/Catch.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
   )
ExternalProject_Get_Property(catch source_dir)
set(CATCH_INCLUDE_DIR ${source_dir}/single_include CACHE INTERNAL "Path to include folder for Catch.")

include_directories(${CATCH_INCLUDE_DIR} "../")

set(SRCS
 "main.cpp"
 "vdf_parser_test.cpp")

add_executable(tests "../vdf_parser.hpp" ${SRCS})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tests) #requires cmake 3.6
add_dependencies(tests catch)
add_definitions("-DSOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")

enable_testing(true)
add_test(NAME vdf_tests COMMAND tests)