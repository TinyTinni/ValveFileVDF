include(CTest)

set (CMAKE_CXX_STANDARD 14)

find_package(Catch2 QUIET)
if (NOT TARGET Catch2::Catch)
    include(ExternalProject)
    find_package(Git REQUIRED)
	
	set(BRANCH "master")
	if (MSVC_VERSION LESS 1900)
		set(BRANCH "Catch1.x") #catch2 needs C++11
	endif()
    
    ExternalProject_Add(
        catch
        PREFIX ${CMAKE_BINARY_DIR}/catch
        GIT_REPOSITORY https://github.com/philsquared/Catch.git
		GIT_TAG ${BRANCH}
        TIMEOUT 10
        UPDATE_COMMAND ${GIT_EXECUTABLE} pull
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
    )
    ExternalProject_Get_Property(catch source_dir)
    
    add_library(Catch INTERFACE)
    set(INCLUDE_POSTFIX "catch2/")
    if (MSVC_VERSION LESS 1900)
        set(INCLUDE_POSTFIX "")
    endif()
    target_include_directories(Catch INTERFACE "${source_dir}/single_include/${INCLUDE_POSTFIX}")
    add_dependencies(Catch catch)
    add_library(Catch2::Catch ALIAS Catch)
endif()

set(SRCS
 "main.cpp"
 "vdf_parser_test.cpp"
 "../Readme.md")

add_executable(tests "../vdf_parser.hpp" ${SRCS})
target_link_libraries(tests PRIVATE Catch2::Catch)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tests) #requires cmake 3.6
add_definitions("-DSOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

if (BUILD_TESTING)
    add_test(NAME vdf_tests COMMAND tests)
endif()
