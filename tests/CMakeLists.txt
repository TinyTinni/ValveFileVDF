
set(SRCS
 "main.cpp"
 "vdf_parser_test.cpp"
 "../Readme.md")

add_executable(tests ${SRCS})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tests) #requires cmake 3.6
set_property(TARGET tests PROPERTY COMPILE_WARNING_AS_ERROR ON)
add_definitions("-DSOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
target_compile_features(tests PUBLIC cxx_std_17)
target_link_libraries(tests PRIVATE ValveFileVDF)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_link_libraries(tests PUBLIC -fsanitize=address,undefined)
endif()

if (BUILD_TESTING)
    add_test(NAME vdf_tests COMMAND tests)
endif()
