include(MiscFunctions)

####
# Temporary workaround for VS toolset changes in 2017
# We need to disable <UseFullPaths> property, but CMake doesn't support it
# until 3.13 (not yet released)
####
if (MSVC)
configure_file(${CATCH_DIR}/tools/misc/SelfTest.vcxproj.user
               ${CMAKE_BINARY_DIR}/tests
               COPYONLY)
endif(MSVC) #Temporary workaround


# define the sources of the self test
# Please keep these ordered alphabetically
set(TEST_SOURCES
        ${SELF_TEST_DIR}/TestMain.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/CmdLine.tests.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/Details.tests.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/GeneratorsImpl.tests.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/InternalBenchmark.tests.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/PartTracker.tests.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/RandomNumberGeneration.tests.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/Tag.tests.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/String.tests.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/StringManip.tests.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/Xml.tests.cpp
        ${SELF_TEST_DIR}/IntrospectiveTests/ToString.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Approx.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/BDD.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Benchmark.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Class.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Compilation.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Condition.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Decomposition.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/EnumToString.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Exception.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Generators.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Message.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Misc.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/ToStringByte.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/ToStringChrono.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/ToStringGeneral.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/ToStringOptional.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/ToStringPair.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/ToStringTuple.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/ToStringVariant.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/ToStringVector.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/ToStringWhich.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Tricky.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/VariadicMacros.tests.cpp
        ${SELF_TEST_DIR}/UsageTests/Matchers.tests.cpp
        )
CheckFileList(TEST_SOURCES ${SELF_TEST_DIR})

# A set of impl files that just #include a single header
# Please keep these ordered alphabetically
set(SURROGATE_SOURCES
        ${SELF_TEST_DIR}/SurrogateCpps/catch_console_colour.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_debugger.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_interfaces_reporter.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_option.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_stream.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_test_case_tracker.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_test_spec.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_xmlwriter.cpp
        )
CheckFileList(SURROGATE_SOURCES ${SELF_TEST_DIR}/SurrogateCpps)


# Specify the headers, too, so CLion recognises them as project files
set(HEADERS
        ${TOP_LEVEL_HEADERS}
        ${EXTERNAL_HEADERS}
        ${INTERNAL_HEADERS}
        ${REPORTER_HEADERS}
        ${BENCHMARK_HEADERS}
        ${BENCHMARK_SOURCES}
)

# Provide some groupings for IDEs
#SOURCE_GROUP("benchmark" FILES ${BENCHMARK_HEADERS} ${BENCHMARK_SOURCES})
#SOURCE_GROUP("Tests" FILES ${TEST_SOURCES})
#SOURCE_GROUP("Surrogates" FILES ${SURROGATE_SOURCES})

include(CTest)

add_executable(SelfTest ${TEST_SOURCES})
target_link_libraries(SelfTest PRIVATE Catch2)

if (CATCH_ENABLE_COVERAGE)
    set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
    find_package(codecov)
    add_coverage(SelfTest)
    list(APPEND LCOV_REMOVE_PATTERNS "'/usr/*'")
    coverage_evaluate()
endif()

# configure unit tests via CTest
add_test(NAME RunTests COMMAND $<TARGET_FILE:SelfTest>)
set_tests_properties(RunTests PROPERTIES
    FAIL_REGULAR_EXPRESSION "Filters:"
    COST 60
)

# Because CTest does not allow us to check both return code _and_ expected
# output in one test, we run these commands twice. First time we check
# the output, the second time we check the exit code.
add_test(NAME List::Tests::Output COMMAND $<TARGET_FILE:SelfTest> --list-tests --verbosity high)
set_tests_properties(List::Tests::Output PROPERTIES
    PASS_REGULAR_EXPRESSION "[0-9]+ test cases"
    FAIL_REGULAR_EXPRESSION "Hidden Test"
)
add_test(NAME List::Tests::ExitCode COMMAND $<TARGET_FILE:SelfTest> --list-tests --verbosity high)
add_test(NAME List::Tests::XmlOutput COMMAND $<TARGET_FILE:SelfTest> --list-tests --verbosity high -r xml)
set_tests_properties(List::Tests::XmlOutput PROPERTIES
    PASS_REGULAR_EXPRESSION "<Line>[0-9]+</Line>"
    FAIL_REGULAR_EXPRESSION "[0-9]+ test cases"
)

add_test(NAME List::Tags::Output COMMAND $<TARGET_FILE:SelfTest> --list-tags)
set_tests_properties(List::Tags::Output PROPERTIES
    PASS_REGULAR_EXPRESSION "[0-9]+ tags"
    FAIL_REGULAR_EXPRESSION "\\[\\.\\]")
add_test(NAME List::Tags::ExitCode COMMAND $<TARGET_FILE:SelfTest> --list-tags)
add_test(NAME List::Tags::XmlOutput COMMAND $<TARGET_FILE:SelfTest> --list-tags -r xml)
set_tests_properties(List::Tags::XmlOutput PROPERTIES
    PASS_REGULAR_EXPRESSION "<Count>18</Count>"
    FAIL_REGULAR_EXPRESSION "[0-9]+ tags"
)


add_test(NAME List::Reporters::Output COMMAND $<TARGET_FILE:SelfTest> --list-reporters)
set_tests_properties(List::Reporters::Output PROPERTIES PASS_REGULAR_EXPRESSION "Available reporters:")
add_test(NAME List::Reporters::ExitCode COMMAND $<TARGET_FILE:SelfTest> --list-reporters)
add_test(NAME List::Reporters::XmlOutput COMMAND $<TARGET_FILE:SelfTest> --list-reporters -r xml)
set_tests_properties(List::Reporters::XmlOutput PROPERTIES
    PASS_REGULAR_EXPRESSION "<Name>compact</Name>"
    FAIL_REGULAR_EXPRESSION "Available reporters:"
)



add_test(NAME NoAssertions COMMAND $<TARGET_FILE:SelfTest> -w NoAssertions "An empty test with no assertions")
set_tests_properties(NoAssertions PROPERTIES PASS_REGULAR_EXPRESSION "No assertions in test case")

add_test(NAME NoTest COMMAND $<TARGET_FILE:SelfTest> Tracker, "___nonexistent_test___")
set_tests_properties(NoTest PROPERTIES
    PASS_REGULAR_EXPRESSION "No test cases matched '___nonexistent_test___'"
    FAIL_REGULAR_EXPRESSION "No tests ran"
)

add_test(NAME WarnAboutNoTests COMMAND ${CMAKE_COMMAND} -P ${SELF_TEST_DIR}/WarnAboutNoTests.cmake $<TARGET_FILE:SelfTest>)

add_test(NAME UnmatchedOutputFilter COMMAND $<TARGET_FILE:SelfTest> [this-tag-does-not-exist] -w NoTests)
set_tests_properties(UnmatchedOutputFilter
  PROPERTIES
    PASS_REGULAR_EXPRESSION "No test cases matched '\\[this-tag-does-not-exist\\]'"
)

add_test(NAME FilteredSection-1 COMMAND $<TARGET_FILE:SelfTest> \#1394 -c RunSection)
set_tests_properties(FilteredSection-1 PROPERTIES FAIL_REGULAR_EXPRESSION "No tests ran")
add_test(NAME FilteredSection-2 COMMAND $<TARGET_FILE:SelfTest> \#1394\ nested -c NestedRunSection -c s1)
set_tests_properties(FilteredSection-2 PROPERTIES FAIL_REGULAR_EXPRESSION "No tests ran")

# AppVeyor has a Python 2.7 in path, but doesn't have .py files as autorunnable
add_test(NAME ApprovalTests COMMAND ${PYTHON_EXECUTABLE} ${CATCH_DIR}/tools/scripts/approvalTests.py $<TARGET_FILE:SelfTest>)
set_tests_properties(ApprovalTests
  PROPERTIES
    FAIL_REGULAR_EXPRESSION "Results differed"
    COST 120 # We know that this is either the most, or second most,
    # expensive test in the test suite, so we give it high estimate for CI runs
)

add_test(NAME RegressionCheck-1670 COMMAND $<TARGET_FILE:SelfTest> "#1670 regression check" -c A -r compact)
set_tests_properties(RegressionCheck-1670 PROPERTIES PASS_REGULAR_EXPRESSION "Passed 1 test case with 2 assertions.")

add_test(NAME VersionCheck COMMAND $<TARGET_FILE:SelfTest> -h)
set_tests_properties(VersionCheck PROPERTIES PASS_REGULAR_EXPRESSION "Catch v${PROJECT_VERSION}")

add_test(NAME LibIdentityTest COMMAND $<TARGET_FILE:SelfTest> --libidentify)
set_tests_properties(LibIdentityTest PROPERTIES PASS_REGULAR_EXPRESSION "description:    A Catch2 test executable")

add_test(NAME FilenameAsTagsTest COMMAND $<TARGET_FILE:SelfTest> -\# --list-tags)
set_tests_properties(FilenameAsTagsTest PROPERTIES PASS_REGULAR_EXPRESSION "\\[#Approx.tests\\]")

add_test(NAME EscapeSpecialCharactersInTestNames COMMAND $<TARGET_FILE:SelfTest> "Test with special\\, characters \"in name")
set_tests_properties(EscapeSpecialCharactersInTestNames PROPERTIES PASS_REGULAR_EXPRESSION "1 assertion in 1 test case")

add_test(NAME NegativeSpecNoHiddenTests COMMAND $<TARGET_FILE:SelfTest> --list-tests ~[approval])
set_tests_properties(NegativeSpecNoHiddenTests PROPERTIES FAIL_REGULAR_EXPRESSION "\\[\\.\\]")

add_test(NAME TestsInFile::SimpleSpecs COMMAND $<TARGET_FILE:SelfTest> "-f ${SELF_TEST_DIR}/Misc/plain-old-tests.input")
set_tests_properties(TestsInFile::SimpleSpecs PROPERTIES PASS_REGULAR_EXPRESSION "6 assertions in 2 test cases")

add_test(NAME TestsInFile::EscapeSpecialCharacters COMMAND $<TARGET_FILE:SelfTest> "-f ${SELF_TEST_DIR}/Misc/special-characters-in-file.input")
set_tests_properties(TestsInFile::EscapeSpecialCharacters PROPERTIES PASS_REGULAR_EXPRESSION "1 assertion in 1 test case")

# CTest does not allow us to create an AND of required regular expressions,
# so we have to split the test into 2 parts and look for parts of the expected
# output separately.
add_test(NAME TestsInFile::InvalidTestNames-1 COMMAND $<TARGET_FILE:SelfTest> "-f ${SELF_TEST_DIR}/Misc/invalid-test-names.input")
set_tests_properties(TestsInFile::InvalidTestNames-1 PROPERTIES PASS_REGULAR_EXPRESSION "Invalid Filter: \"Test with special, characters in \\\\\" name\"")

add_test(NAME TestsInFile::InvalidTestNames-2 COMMAND $<TARGET_FILE:SelfTest> "-f ${SELF_TEST_DIR}/Misc/invalid-test-names.input")
set_tests_properties(TestsInFile::InvalidTestNames-2 PROPERTIES PASS_REGULAR_EXPRESSION "No tests ran")

add_test(NAME TagAlias COMMAND $<TARGET_FILE:SelfTest> [@tricky] --list-tests)
set_tests_properties(TagAlias PROPERTIES
  PASS_REGULAR_EXPRESSION "[0-9]+ matching test cases"
  FAIL_REGULAR_EXPRESSION "0 matching test cases"
)

if (CATCH_USE_VALGRIND)
    add_test(NAME ValgrindRunTests COMMAND valgrind --leak-check=full --error-exitcode=1 $<TARGET_FILE:SelfTest>)
    add_test(NAME ValgrindListTests COMMAND valgrind --leak-check=full --error-exitcode=1 $<TARGET_FILE:SelfTest> --list-tests --verbosity high)
    set_tests_properties(ValgrindListTests PROPERTIES PASS_REGULAR_EXPRESSION "definitely lost: 0 bytes in 0 blocks")
    add_test(NAME ValgrindListTags COMMAND valgrind --leak-check=full --error-exitcode=1 $<TARGET_FILE:SelfTest> --list-tags)
    set_tests_properties(ValgrindListTags PROPERTIES PASS_REGULAR_EXPRESSION "definitely lost: 0 bytes in 0 blocks")
endif()

list(APPEND CATCH_WARNING_TARGETS SelfTest)
set(CATCH_WARNING_TARGETS ${CATCH_WARNING_TARGETS} PARENT_SCOPE)
