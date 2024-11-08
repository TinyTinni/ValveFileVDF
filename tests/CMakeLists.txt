
set(SRCS
 "main.cpp"
 "vdf_parser_test.cpp"
 "../Readme.md")

add_executable(tests ${SRCS})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tests) #requires cmake 3.6
add_definitions("-DSOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
target_compile_features(tests PUBLIC cxx_std_17)
target_link_libraries(tests PRIVATE ValveFileVDF)

target_compile_options(tests PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -Wconversion -pedantic-errors -Wsign-conversion>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_link_libraries(tests PUBLIC -fsanitize=address,undefined)
endif()


add_test(NAME vdf_tests COMMAND tests --order-by=rand)

add_subdirectory(proptests)

