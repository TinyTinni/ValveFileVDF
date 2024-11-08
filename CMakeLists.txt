cmake_minimum_required (VERSION 3.23)

project(
  ValveFileVDF
  VERSION 1.0.0
  DESCRIPTION "parses valve's vdf file format"
  HOMEPAGE_URL "https://github.com/TinyTinni/ValveFileVDF"
  LANGUAGES CXX C)

option(VALVEFILEVDF_ENABLE_TESTING OFF)
option(VALVEFILEVDF_ENABLE_FUZZING OFF)
option(VALVEFILEVDF_ENABLE_PYTHON OFF)

if (VALVEFILEVDF_ENABLE_FUZZING)
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC" AND NOT APPLE)
	    set(VALVEFILEVDF_ENABLE_FUZZING ON)
    else()
    set(VALVEFILEVDF_ENABLE_FUZZING OFF)
        message("Fuzzing not supported. Fuzzing gets disabled.")
    endif()
endif()

add_library(ValveFileVDF INTERFACE)
add_library(ValveFileVDF::ValveFileVDF ALIAS ValveFileVDF)
target_sources(ValveFileVDF
    PUBLIC FILE_SET HEADERS
    BASE_DIRS "include"
    FILES
    "include/vdf_parser.hpp"
    )

#############################
## Install

set_target_properties(ValveFileVDF PROPERTIES PUBLIC_HEADER "include/vdf_parser.hpp")

include(GNUInstallDirs)
install(TARGETS ValveFileVDF
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

include(CMakePackageConfigHelpers)


write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION cmake
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION cmake
)


###################################
# Tests
if (${VALVEFILEVDF_ENABLE_TESTING} OR ${VALVEFILEVDF_ENABLE_FUZZING})
    include(CTest)
endif()

if (${VALVEFILEVDF_ENABLE_TESTING})
    add_subdirectory(./tests)
endif()


if (${VALVEFILEVDF_ENABLE_FUZZING})
    add_subdirectory(fuzzing)
endif()

if (${VALVEFILEVDF_ENABLE_PYTHON})
    add_subdirectory(python)
endif()