# CI and build system integration

Build Systems may refer to low-level tools, like CMake, or larger systems that run on servers, like Jenkins or TeamCity. This page will talk about both.

# Continuous Integration systems

Probably the most important aspect to using Catch with a build server is the use of different reporters. Catch comes bundled with three reporters that should cover the majority of build servers out there - although adding more for better integration with some is always a possibility (currently we also offer TeamCity, TAP and Automake reporters).

Two of these reporters are built in (XML and JUnit) and the third (TeamCity) is included as a separate header. It's possible that the other two may be split out in the future too - as that would make the core of Catch smaller for those that don't need them.

## XML Reporter
```-r xml``` 

The XML Reporter writes in an XML format that is specific to Catch. 

The advantage of this format is that it corresponds well to the way Catch works (especially the more unusual features, such as nested sections) and is a fully streaming format - that is it writes output as it goes, without having to store up all its results before it can start writing.

The disadvantage is that, being specific to Catch, no existing build servers understand the format natively. It can be used as input to an XSLT transformation that could covert it to, say, HTML - although this loses the streaming advantage, of course.

## JUnit Reporter
```-r junit```

The JUnit Reporter writes in an XML format that mimics the JUnit ANT schema.

The advantage of this format is that the JUnit Ant schema is widely understood by most build servers and so can usually be consumed with no additional work.

The disadvantage is that this schema was designed to correspond to how JUnit works - and there is a significant mismatch with how Catch works. Additionally the format is not streamable (because opening elements hold counts of failed and passing tests as attributes) - so the whole test run must complete before it can be written.

## Other reporters
Other reporters are not part of the single-header distribution and need to be downloaded and included separately. All reporters are stored in `include/reporters` directory in the git repository, and are named `catch_reporter_*.hpp`. For example, to use the TeamCity reporter you need to download `include/reporters/catch_reporter_teamcity.hpp` and include it after Catch itself.

```
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "catch_reporter_teamcity.hpp"
```

### TeamCity Reporter
```-r teamcity```

The TeamCity Reporter writes TeamCity service messages to stdout. In order to be able to use this reporter an additional header must also be included.

Being specific to TeamCity this is the best reporter to use with it - but it is completely unsuitable for any other purpose. It is a streaming format (it writes as it goes) - although test results don't appear in the TeamCity interface until the completion of a suite (usually the whole test run).

### Automake Reporter
```-r automake```

The Automake Reporter writes out the [meta tags](https://www.gnu.org/software/automake/manual/html_node/Log-files-generation-and-test-results-recording.html#Log-files-generation-and-test-results-recording) expected by automake via `make check`.

### TAP (Test Anything Protocol) Reporter
```-r tap```

Because of the incremental nature of Catch's test suites and ability to run specific tests, our implementation of TAP reporter writes out the number of tests in a suite last.

# Low-level tools

## CMake

In general we recommend "vendoring" Catch's single-include releases inside your own repository. If you do this, the following example shows a minimal CMake project:
```CMake
cmake_minimum_required(VERSION 3.0)

project(cmake_test)

# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/catch)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Make test executable
set(TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests Catch)
```
Note that it assumes that the path to the Catch's header is `catch/catch.hpp` from the `CMakeLists.txt` file.


You can also use the following CMake snippet to automatically fetch the entire Catch repository from github and configure it as an external project:
```CMake
cmake_minimum_required(VERSION 2.8.8)
project(catch_builder CXX)
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

# Expose required variable (CATCH_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(catch source_dir)
set(CATCH_INCLUDE_DIR ${source_dir}/single_include CACHE INTERNAL "Path to include folder for Catch")
```

If you put it in, e.g., `${PROJECT_SRC_DIR}/${EXT_PROJECTS_DIR}/catch/`, you can use it in your project by adding the following to your root CMake file:

```CMake
# Includes Catch in the project:
add_subdirectory(${EXT_PROJECTS_DIR}/catch)
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})
enable_testing(true)  # Enables unit-testing.
```

The advantage of this approach is that you can always automatically update Catch to the latest release. The disadvantage is that it means bringing in lot more than you need.


### Automatic test registration
If you are also using ctest, `contrib/ParseAndAddCatchTests.cmake` is a CMake script that attempts to parse your test files and automatically register all test cases, using tags as labels. This means that these
```cpp
TEST_CASE("Test1", "[unit]") {
    int a = 1;
    int b = 2;
    REQUIRE(a == b);
}

TEST_CASE("Test2") {
    int a = 1;
    int b = 2;
    REQUIRE(a == b);
}

TEST_CASE("Test3", "[a][b][c]") {
    int a = 1;
    int b = 2;
    REQUIRE(a == b);
}
```
would be registered as 3 tests, `Test1`, `Test2` and `Test3`, and ctest 4 labels would be created, `a`, `b`, `c` and `unit`.

### CodeCoverage module (GCOV, LCOV...)

If you are using GCOV tool to get testing coverage of your code, and are not sure how to integrate it with CMake and Catch, there should be an external example over at https://github.com/fkromer/catch_cmake_coverage

---

[Home](Readme.md)
