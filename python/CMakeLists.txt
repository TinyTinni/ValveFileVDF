
include (../cmake/get_cpm.cmake)

CPMAddPackage(NAME pybind11
    GITHUB_REPOSITORY "pybind/pybind11"
    GIT_TAG "v2.13.6"
)

pybind11_add_module(vdf "vdf_python_bindings.cpp")
target_link_libraries(vdf PRIVATE pybind11::pybind11 ValveFileVDF)
target_compile_features(vdf PUBLIC cxx_std_17)
install(TARGETS vdf DESTINATION .)

## add tests
if (VALVEFILEVDF_ENABLE_TESTING)
    include(CTest)
    add_test(NAME vdf_python_test COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test.py WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../tests/testdata")
    set(ppath $ENV{PYTHONPATH})
    LIST(APPEND ppath  "$<TARGET_FILE_DIR:vdf>")
    set_tests_properties(vdf_python_test PROPERTIES ENVIRONMENT "PYTHONPATH=${ppath}" DEPENDS vdf)
endif()
