set (CMAKE_CXX_STANDARD 17)

##find_package(Catch2 QUIET)
##if (NOT TARGET Catch2::Catch)
##    include(ExternalProject)
##    find_package(Git REQUIRED)
##	
##	set(BRANCH "master")
##	if (MSVC_VERSION LESS 1900)
##		set(BRANCH "Catch1.x") #catch2 needs C++11
##	endif()
##    
##    ExternalProject_Add(
##        catch
##        PREFIX ${CMAKE_BINARY_DIR}/catch
##        GIT_REPOSITORY https://github.com/philsquared/Catch.git
##		GIT_TAG ${BRANCH}
##        TIMEOUT 10
##        UPDATE_COMMAND ${GIT_EXECUTABLE} pull
##        CONFIGURE_COMMAND ""
##        BUILD_COMMAND ""
##        INSTALL_COMMAND ""
##        LOG_DOWNLOAD ON
##    )
##    ExternalProject_Get_Property(catch source_dir)
##    
##    add_library(Catch INTERFACE)
##    set(INCLUDE_POSTFIX "catch2/")
##    if (MSVC_VERSION LESS 1900)
##        set(INCLUDE_POSTFIX "")
##    endif()
##    target_include_directories(Catch INTERFACE "${source_dir}/single_include/${INCLUDE_POSTFIX}")
##    add_dependencies(Catch catch)
##    add_library(Catch2::Catch ALIAS Catch)
##endif()

set(SRCS
 "main.cpp"
 "vdf_parser_test.cpp"
 "catch.cpp"
 "../Readme.md")

add_executable(tests ${SRCS})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tests) #requires cmake 3.6
set_property(TARGET tests PROPERTY COMPILE_WARNING_AS_ERROR ON)
add_definitions("-DSOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"" "-DCATCH_AMALGAMATED_CUSTOM_MAIN")
target_compile_definitions(tests PRIVATE "-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS" "-D_LIBCPP_DISABLE_DEPRECATION_WARNINGS")
target_link_libraries(tests PRIVATE ValveFileVDF)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_link_libraries(tests PUBLIC -fsanitize=address,undefined)
endif()

if (BUILD_TESTING)
    add_test(NAME vdf_tests COMMAND tests)
endif()
