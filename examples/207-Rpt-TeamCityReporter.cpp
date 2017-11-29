// 207-Rpt-TeamCityReporter.cpp

// Catch has built-in and external reporters:
// Built-in:
// - compact
// - console
// - junit
// - xml
// External:
// - automake
// - tap
// - teamcity (this example)

// main() and reporter code provided in 200-Rpt-CatchMain.cpp

#include <catch2/catch.hpp>

#ifdef _MSC_VER
# pragma warning (disable : 4702) // Disable warning: unreachable code
#endif

TEST_CASE( "TeamCity passes unconditionally succeeding assertion", "[teamcity]" ) {

    SUCCEED();
}

TEST_CASE( "TeamCity reports unconditionally failing assertion", "[teamcity]" ) {

    FAIL();
}

TEST_CASE( "TeamCity reports failing check", "[teamcity]" ) {

    REQUIRE( 3 == 7 );
}

TEST_CASE( "TeamCity reports failing check-false", "[teamcity]" ) {

    REQUIRE_FALSE( 3 == 3 );
}

TEST_CASE( "TeamCity reports failing check-that", "[teamcity]" ) {

    using namespace Catch;

    REQUIRE_THAT( "hello", Contains( "world" ) );
}

TEST_CASE( "TeamCity reports unexpected exception", "[teamcity]" ) {

    REQUIRE( (throw std::runtime_error("surprise!"), true) );
}

TEST_CASE( "TeamCity reports undesired exception", "[teamcity]" ) {

    REQUIRE_NOTHROW( (throw std::runtime_error("surprise!"), true) );
}

TEST_CASE( "TeamCity reports missing expected exception", "[teamcity]" ) {

    REQUIRE_THROWS( true );
}

TEST_CASE( "TeamCity reports missing specific expected exception", "[teamcity]" ) {

    REQUIRE_THROWS_AS( throw std::bad_alloc(), std::runtime_error );
}

TEST_CASE( "TeamCity reports unexpected message in expected exception", "[teamcity]" ) {

    using namespace Catch;

    CHECK_THROWS_WITH( throw std::runtime_error("hello"), "world" );
    CHECK_THROWS_WITH( throw std::runtime_error("hello"), Contains("world") );
}

struct MyException: public std::runtime_error
{
    MyException( char const * text )
    : std::runtime_error( text ) {}
    
    ~MyException() override;
};

// prevent -Wweak-vtables:
MyException::~MyException() = default;

struct MyExceptionMatcher : Catch::MatcherBase< std::runtime_error >
{
    std::string m_text;

    MyExceptionMatcher( char const * text )
    : m_text( text )
    {}

    ~MyExceptionMatcher() override;
    
    bool match( std::runtime_error const & arg ) const override
    { 
        return m_text == arg.what() ; 
    }
    
    std::string describe() const override 
    { 
        return "it's me";
    }
};

// prevent -Wweak-vtables:
MyExceptionMatcher::~MyExceptionMatcher() = default;

TEST_CASE( "TeamCity failing check-throws-matches", "[teamcity]" ) {

    CHECK_THROWS_MATCHES( throw MyException("hello"), MyException, MyExceptionMatcher("world") );
}

// [!throws] - lets Catch know that this test is likely to throw an exception even if successful. 
// This causes the test to be excluded when running with -e or --nothrow.

// No special effects for the reporter.

TEST_CASE( "TeamCity throwing exception with tag [!throws]", "[teamcity][!throws]" ) {

    REQUIRE_THROWS( throw std::runtime_error("unsurprisingly") );
}

// [!mayfail] - doesn't fail the test if any given assertion fails (but still reports it). This can be useful to flag a work-in-progress, or a known issue that you don't want to immediately fix but still want to track in your tests.

TEST_CASE( "TeamCity failing assertion with tag [!mayfail]", "[teamcity][!mayfail] " ) {

    REQUIRE( 3 == 7 );  // doesn't fail test case this time, reports: testIgnored
    REQUIRE( 3 == 3 );
}

// [!shouldfail] - like [!mayfail] but fails the test if it passes. 
// This can be useful if you want to be notified of accidental, or third-party, fixes.

TEST_CASE( "TeamCity succeeding assertion with tag [!shouldfail]", "[teamcity][!shouldfail]" ) {

    SUCCEED( "Marked [!shouldfail]" );
}

// Compile & run:
// - g++ -std=c++11 -Wall -I$(CATCH_ROOT) -DCATCH_EXAMPLE_RPT_1=\"include/reporters/catch_reporter_teamcity.hpp\" -o 200-Rpt-CatchMainTeamCity.o -c 200-Rpt-CatchMain.cpp
// - g++ -std=c++11 -Wall -I$(CATCH_ROOT) -o 207-Rpt-TeamCityReporter 207-Rpt-TeamCityReporter.cpp 200-Rpt-CatchMainTeamCity.o && 207-Rpt-TeamCityReporter --list-reporters
//
// - cl -EHsc -I%CATCH_ROOT% -DCATCH_EXAMPLE_RPT_1=\"include/reporters/catch_reporter_teamcity.hpp\" -Fo200-Rpt-CatchMainTeamCity.obj -c 200-Rpt-CatchMain.cpp
// - cl -EHsc -I%CATCH_ROOT% 207-Rpt-TeamCityReporter.cpp 200-Rpt-CatchMainTeamCity.o && 207-Rpt-TeamCityReporter --list-reporters

// Compilation output (--list-reporters):
// Available reporters:
//   compact:   Reports test results on a single line, suitable for IDEs
//   console:   Reports test results as plain lines of text
//   junit:     Reports test results in an XML format that looks like Ant's
//                junitreport target
//   teamcity:  Reports test results as TeamCity service messages
//   xml:       Reports test results as an XML document

// Expected output (abbreviated and broken into shorter lines):
//
// prompt> 207-Rpt-TeamCityReporter.exe --reporter teamcity
// ##teamcity[testSuiteStarted name='207-Rpt-TeamCityReporter.exe']
// ##teamcity[testStarted name='TeamCity passes unconditionally succeeding assertion']
// ##teamcity[testFinished name='TeamCity passes unconditionally succeeding assertion' duration='1']
// ##teamcity[testStarted name='TeamCity reports unconditionally failing assertion']
// ##teamcity[testFailed name='TeamCity reports unconditionally failing assertion' /
// message='.../examples/207-Rpt-TeamCityReporter.cpp:23|n/
// ...............................................................................|n|n/
// .../examples/207-Rpt-TeamCityReporter.cpp:25|nexplicit failure']
// ##teamcity[testFinished name='TeamCity reports unconditionally failing assertion' duration='3']
// ...
