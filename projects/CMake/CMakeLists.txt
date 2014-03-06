cmake_minimum_required(VERSION 2.8)

project(Catch)

# define some folders
get_filename_component(CATCH_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
get_filename_component(CATCH_DIR "${CATCH_DIR}" PATH)
set(SELF_TEST_DIR ${CATCH_DIR}/projects/SelfTest)

# define the sources of the self test
set(SOURCES
    ${SELF_TEST_DIR}/ApproxTests.cpp
    ${SELF_TEST_DIR}/BDDTests.cpp
    ${SELF_TEST_DIR}/ClassTests.cpp
    ${SELF_TEST_DIR}/ConditionTests.cpp
    ${SELF_TEST_DIR}/ExceptionTests.cpp
    ${SELF_TEST_DIR}/GeneratorTests.cpp
    ${SELF_TEST_DIR}/MessageTests.cpp
    ${SELF_TEST_DIR}/MiscTests.cpp
    ${SELF_TEST_DIR}/SectionTrackerTests.cpp
    ${SELF_TEST_DIR}/TestMain.cpp
    ${SELF_TEST_DIR}/TrickyTests.cpp
    ${SELF_TEST_DIR}/VariadicMacrosTests.cpp
)

# configure the executable
include_directories(${CATCH_DIR}/include)
add_executable(SelfTest ${SOURCES})

# configure unit tests via CTest
enable_testing()
add_test(NAME RunTests COMMAND SelfTest)

add_test(NAME ListTests COMMAND SelfTest --list-tests)
set_tests_properties(ListTests PROPERTIES PASS_REGULAR_EXPRESSION "[0-9]+ test cases")

add_test(NAME ListTags COMMAND SelfTest --list-tags)
set_tests_properties(ListTags PROPERTIES PASS_REGULAR_EXPRESSION "[0-9]+ tags")
