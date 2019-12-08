cmake_minimum_required( VERSION 3.5 )

project( Catch2Examples LANGUAGES CXX )

message( STATUS "Examples included" )


# Some one-offs first:
# 1) Tests and main in one file
add_executable( 010-TestCase
  010-TestCase.cpp
)

# 2) Tests and main across two files
add_executable( 020-MultiFile
  020-TestCase-1.cpp
  020-TestCase-2.cpp
)

# TODO: How to handle differently configured libraries?
#       Needed in 231-Cfg-OutputStreams

# These examples use the standard separate compilation
set( SOURCES_IDIOMATIC_EXAMPLES
    030-Asn-Require-Check.cpp
    100-Fix-Section.cpp
    110-Fix-ClassFixture.cpp
    120-Bdd-ScenarioGivenWhenThen.cpp
    210-Evt-EventListeners.cpp
    300-Gen-OwnGenerator.cpp
    301-Gen-MapTypeConversion.cpp
    310-Gen-VariablesInGenerators.cpp
    311-Gen-CustomCapture.cpp
)

string( REPLACE ".cpp" "" BASENAMES_IDIOMATIC_EXAMPLES "${SOURCES_IDIOMATIC_EXAMPLES}" )
set( TARGETS_IDIOMATIC_EXAMPLES ${BASENAMES_IDIOMATIC_EXAMPLES} )


foreach( name ${TARGETS_IDIOMATIC_EXAMPLES} )
    add_executable( ${name}
      000-CatchMain.cpp
      ${EXAMPLES_DIR}/${name}.cpp )
endforeach()

set(ALL_EXAMPLE_TARGETS
  ${TARGETS_IDIOMATIC_EXAMPLES}
  010-TestCase
  020-MultiFile
)

foreach( name ${ALL_EXAMPLE_TARGETS} )
    target_link_libraries( ${name} Catch2 )
    set_property(TARGET ${name} PROPERTY CXX_STANDARD 14)
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
