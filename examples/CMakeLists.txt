#
# Build examples.
#
# Requires CATCH_BUILD_EXAMPLES to be defined 'true', see ../CMakeLists.txt.
#

cmake_minimum_required( VERSION 3.0 )

project( CatchExamples CXX )

message( STATUS "Examples included" )

# define folders used:

set( EXAMPLES_DIR ${CATCH_DIR}/examples )
set( HEADER_DIR   ${CATCH_DIR}/single_include )
set( REPORTER_HEADER_DIR ${CATCH_DIR}/include/reporters )

# single-file sources:

set( SOURCES_SINGLE_FILE
    010-TestCase.cpp
    231-Cfg-OutputStreams.cpp
)

# multiple-file modules:

set( SOURCES_020
    020-TestCase-1.cpp
    020-TestCase-2.cpp
)

# main for idiomatic test sources:

set( SOURCES_IDIOMATIC_MAIN
    000-CatchMain.cpp
)

# sources to combine with 000-CatchMain.cpp:

set( SOURCES_IDIOMATIC_TESTS
    030-Asn-Require-Check.cpp
    100-Fix-Section.cpp
    110-Fix-ClassFixture.cpp
    120-Bdd-ScenarioGivenWhenThen.cpp
    210-Evt-EventListeners.cpp
)

# main-s for reporter-specific test sources:

set( SOURCES_REPORTERS_MAIN
    200-Rpt-CatchMain.cpp
)

string( REPLACE ".cpp" "" BASENAMES_REPORTERS_MAIN 200-Rpt-CatchMain.cpp )

set( NAMES_REPORTERS TeamCity )

foreach( reporter ${NAMES_REPORTERS} )
    list( APPEND SOURCES_SPECIFIC_REPORTERS_MAIN ${BASENAMES_REPORTERS_MAIN}${reporter}.cpp )
endforeach()

# sources to combine with 200-Rpt-CatchMain{Reporter}.cpp:

set( SOURCES_REPORTERS_TESTS
    207-Rpt-TeamCityReporter.cpp
)

# check if all sources are listed, warn if not:

set( SOURCES_ALL
    ${SOURCES_020}
    ${SOURCES_SINGLE_FILE}
    ${SOURCES_IDIOMATIC_MAIN}
    ${SOURCES_IDIOMATIC_TESTS}
    ${SOURCES_REPORTERS_MAIN}
    ${SOURCES_REPORTERS_TESTS}
)

foreach( name ${SOURCES_ALL} )
    list( APPEND SOURCES_ALL_PATH ${EXAMPLES_DIR}/${name} )
endforeach()

CheckFileList( SOURCES_ALL_PATH ${EXAMPLES_DIR} )

# create target names:

string( REPLACE ".cpp" "" BASENAMES_SINGLE_FILE     "${SOURCES_SINGLE_FILE}" )
string( REPLACE ".cpp" "" BASENAMES_IDIOMATIC_TESTS "${SOURCES_IDIOMATIC_TESTS}" )
string( REPLACE ".cpp" "" BASENAMES_REPORTERS_TESTS "${SOURCES_REPORTERS_TESTS}" )
string( REPLACE ".cpp" "" BASENAMES_REPORTERS_MAIN  "${SOURCES_REPORTERS_MAIN}" )

set( TARGETS_SINGLE_FILE     ${BASENAMES_SINGLE_FILE} )
set( TARGETS_IDIOMATIC_TESTS ${BASENAMES_IDIOMATIC_TESTS} )
set( TARGETS_REPORTERS_TESTS ${BASENAMES_REPORTERS_TESTS} )
set( TARGETS_REPORTERS_MAIN  ${BASENAMES_REPORTERS_MAIN} )

set( TARGETS_ALL
    ${TARGETS_SINGLE_FILE}
    020-TestCase
    ${TARGETS_IDIOMATIC_TESTS} CatchMain
    ${TARGETS_REPORTERS_TESTS} CatchMainTeamCity
)

# define program targets:

add_library( CatchMain         OBJECT ${EXAMPLES_DIR}/${SOURCES_IDIOMATIC_MAIN} ${HEADER_DIR}/catch2/catch.hpp )
#add_library( CatchMainAutomake OBJECT ${EXAMPLES_DIR}/200-Rpt-CatchMain.cpp ${HEADER_DIR}/catch2/catch.hpp )
#add_library( CatchMainTap      OBJECT ${EXAMPLES_DIR}/200-Rpt-CatchMain.cpp ${HEADER_DIR}/catch2/catch.hpp )
add_library( CatchMainTeamCity OBJECT ${EXAMPLES_DIR}/200-Rpt-CatchMain.cpp ${HEADER_DIR}/catch2/catch.hpp )

#target_compile_definitions( CatchMainAutomake PRIVATE CATCH_EXAMPLE_RPT_1=\"include/reporters/catch_reporter_automake.hpp\" )
#target_compile_definitions( CatchMainTap      PRIVATE CATCH_EXAMPLE_RPT_1=\"include/reporters/catch_reporter_tap.hpp\" )
target_compile_definitions( CatchMainTeamCity PRIVATE CATCH_EXAMPLE_RPT_1=\"include/reporters/catch_reporter_teamcity.hpp\" )

foreach( name ${TARGETS_SINGLE_FILE} )
    add_executable( ${name} ${EXAMPLES_DIR}/${name}.cpp ${HEADER_DIR}/catch2/catch.hpp )
endforeach()

foreach( name ${TARGETS_IDIOMATIC_TESTS} )
    add_executable( ${name} ${EXAMPLES_DIR}/${name}.cpp $<TARGET_OBJECTS:CatchMain> ${HEADER_DIR}/catch2/catch.hpp )
endforeach()

add_executable( 020-TestCase ${EXAMPLES_DIR}/020-TestCase-1.cpp ${EXAMPLES_DIR}/020-TestCase-2.cpp ${HEADER_DIR}/catch2/catch.hpp )

#add_executable( 207-Rpt-AutomakeReporter ${EXAMPLES_DIR}/207-Rpt-AutomakeReporter.cpp $<TARGET_OBJECTS:CatchMainAutomake> ${HEADER_DIR}/catch2/catch.hpp )
#add_executable( 207-Rpt-TapReporter      ${EXAMPLES_DIR}/207-Rpt-TapReporter.cpp      $<TARGET_OBJECTS:CatchMainTap>      ${HEADER_DIR}/catch2/catch.hpp )
add_executable( 207-Rpt-TeamCityReporter ${EXAMPLES_DIR}/207-Rpt-TeamCityReporter.cpp $<TARGET_OBJECTS:CatchMainTeamCity> ${HEADER_DIR}/catch2/catch.hpp )

#foreach( name ${TARGETS_REPORTERS_TESTS} )
#    add_executable( ${name} ${EXAMPLES_DIR}/${name}.cpp $<TARGET_OBJECTS:CatchMain> ${HEADER_DIR}/catch2/catch.hpp )
#endforeach()

foreach( name ${TARGETS_ALL} )
    target_include_directories( ${name} PRIVATE ${HEADER_DIR} ${CATCH_DIR} )

    set_property(TARGET ${name} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${name} PROPERTY CXX_EXTENSIONS OFF)

    # Add desired warnings
    if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
        target_compile_options( ${name}  PRIVATE -Wall -Wextra -Wunreachable-code )
    endif()
    # Clang specific warning go here
    if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
        # Actually keep these
        target_compile_options( ${name}  PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
    endif()
    if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
        target_compile_options( ${name}  PRIVATE /W4 /w44265 /WX )
    endif()
endforeach()

