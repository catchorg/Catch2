cmake_minimum_required(VERSION 2.8)

project(Catch C CXX)
message("configure: Catch/SelfTest")

# define some folders
set(CATCH_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(SELF_TEST_DIR ${CATCH_DIR}/projects/SelfTest)
set(SCRIPTS_DIR   ${CATCH_DIR}/scripts)

# define the sources of the self test
set(
    SOURCES
	${SELF_TEST_DIR}/ApproxTests.cpp
	${SELF_TEST_DIR}/BDDTests.cpp
	${SELF_TEST_DIR}/catch_self_test.cpp
	${SELF_TEST_DIR}/ClassTests.cpp
	${SELF_TEST_DIR}/CmdLineTests.cpp
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

# configure unit tests via ctest
enable_testing()
add_test(NAME SelfTest_run COMMAND SelfTest)
#add_test(NAME SelfTest_run COMMAND python ${SCRIPTS_DIR}/approvalTests.py "${CMAKE_CURRENT_BINARY_DIR}/SelfTest")
