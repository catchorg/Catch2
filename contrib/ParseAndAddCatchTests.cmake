#==================================================================================================#
#  supported macros                                                                                #
#    - TEST_CASE,                                                                                  #
#    - SCENARIO,                                                                                   #
#    - TEST_CASE_METHOD,                                                                           #
#    - CATCH_TEST_CASE,                                                                            #
#    - CATCH_SCENARIO,                                                                             #
#    - CATCH_TEST_CASE_METHOD.                                                                     #
#                                                                                                  #
#  Usage                                                                                           #
# 1. make sure this module is in the path or add this otherwise:                                   #
#    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake.modules/")              #
# 2. make sure that you've enabled testing option for the project by the call:                     #
#    enable_testing()                                                                              #
# 3. add the lines to the script for testing target (sample CMakeLists.txt):                       #
#        project(testing_target)                                                                   #
#        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake.modules/")          #
#        enable_testing()                                                                          #
#                                                                                                  #
#        find_path(CATCH_INCLUDE_DIR "catch.hpp")                                                  #
#        include_directories(${INCLUDE_DIRECTORIES} ${CATCH_INCLUDE_DIR})                          #
#                                                                                                  #
#        file(GLOB SOURCE_FILES "*.cpp")                                                           #
#        add_executable(${PROJECT_NAME} ${SOURCE_FILES})                                           #
#                                                                                                  #
#        include(ParseAndAddCatchTests)                                                            #
#        ParseAndAddCatchTests(${PROJECT_NAME})                                                    #
#==================================================================================================#

cmake_minimum_required(VERSION 2.8.8)

# This removes the contents between
#  - block comments (i.e. /* ... */) 
#  - full line comments (i.e. // ... ) 
# contents have been read into '${CppCode}'.
# !keep partial line comments
function(RemoveComments CppCode)
  string(ASCII 2 CMakeBeginBlockComment)
  string(ASCII 3 CMakeEndBlockComment)
  string(REGEX REPLACE "/\\*" "${CMakeBeginBlockComment}" ${CppCode} "${${CppCode}}")
  string(REGEX REPLACE "\\*/" "${CMakeEndBlockComment}" ${CppCode} "${${CppCode}}")
  string(REGEX REPLACE "${CMakeBeginBlockComment}[^${CMakeEndBlockComment}]*${CMakeEndBlockComment}" "" ${CppCode} "${${CppCode}}")
  string(REGEX REPLACE "\n[ \t]*//+[^\n]+" "\n" ${CppCode} "${${CppCode}}")

  set(${CppCode} "${${CppCode}}" PARENT_SCOPE)
endfunction()

# Worker function
function(ParseFile SourceFile TestTarget)
    if(NOT EXISTS ${SourceFile})
        return()
    endif()
    file(STRINGS ${SourceFile} Contents NEWLINE_CONSUME)

    # Remove block and fullline comments
    RemoveComments(Contents)

    # Find definition of test names
    string(REGEX MATCHALL "[ \t]*(CATCH_)?(TEST_CASE_METHOD|SCENARIO|TEST_CASE)[ \t]*\\([^\)]+\\)+[ \t\n]*{+[ \t]*(//[^\n]*[Tt][Ii][Mm][Ee][Oo][Uu][Tt][ \t]*[0-9]+)*" Tests "${Contents}")

    foreach(TestName ${Tests})
        # Strip newlines
        string(REGEX REPLACE "\\\\\n|\n" "" TestName "${TestName}")

        # Get test type and fixture if applicable
        string(REGEX MATCH "(CATCH_)?(TEST_CASE_METHOD|SCENARIO|TEST_CASE)[ \t]*\\([^,^\"]*" TestTypeAndFixture "${TestName}")
        string(REGEX MATCH "(CATCH_)?(TEST_CASE_METHOD|SCENARIO|TEST_CASE)" TestType "${TestTypeAndFixture}")
        string(REPLACE "${TestType}(" "" TestFixture "${TestTypeAndFixture}")

        # Get string parts of test definition
        string(REGEX MATCHALL "\"+([^\\^\"]|\\\\\")+\"+" TestStrings "${TestName}")

        # Strip wrapping quotation marks
        string(REGEX REPLACE "^\"(.*)\"$" "\\1" TestStrings "${TestStrings}")
        string(REPLACE "\";\"" ";" TestStrings "${TestStrings}")

        # Validate that a test name and tags have been provided
        list(LENGTH TestStrings TestStringsLength)
        if(TestStringsLength GREATER 2 OR TestStringsLength LESS 1)
            message(FATAL_ERROR "You must provide a valid test name and tags for all tests in ${SourceFile}")
        endif()

        # Assign name and tags
        list(GET TestStrings 0 Name)
        if("${TestType}" STREQUAL "SCENARIO")
            set(Name "Scenario: ${Name}")
        endif()
        if(TestFixture)
            set(CTestName "${TestFixture}:${Name}")
        else()
            set(CTestName "${Name}")
        endif()
        if(TestStringsLength EQUAL 2)
            list(GET TestStrings 1 Tags)
            string(TOLOWER "${Tags}" Tags)
            string(REPLACE "]" ";" Tags "${Tags}")
            string(REPLACE "[" "" Tags "${Tags}")
        endif()

        # Add the test and set its properties
        add_test(NAME "\"${CTestName}\"" COMMAND ${TestTarget} ${Name} ${AdditionalCatchParameters})
        set_tests_properties("\"${CTestName}\"" PROPERTIES FAIL_REGULAR_EXPRESSION "No tests ran"
                                                LABELS "${Tags}")

    endforeach()
endfunction()

# entry point
function(ParseAndAddCatchTests TestTarget)
    get_target_property(SourceFiles ${TestTarget} SOURCES)
    foreach(SourceFile ${SourceFiles})
        ParseFile(${SourceFile} ${TestTarget})
    endforeach()
endfunction()