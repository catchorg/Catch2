/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#pragma clang diagnostic ignored "-Wpadded"

#include "catch_self_test.hpp"

TEST_CASE( "selftest/main", "Runs all Catch self tests and checks their results" ) {
    using namespace Catch;
    
    ///////////////////////////////////////////////////////////////////////////
    SECTION(    "selftest/expected result",
                "Tests do what they claim" ) {
                
        SECTION(    "selftest/expected result/failing tests", 
                    "Tests in the 'failing' branch fail" ) {
            MetaTestRunner::runMatching( "./failing/*",  MetaTestRunner::Expected::ToFail );
        }
        
        SECTION(    "selftest/expected result/succeeding tests", 
                    "Tests in the 'succeeding' branch succeed" ) {
            MetaTestRunner::runMatching( "./succeeding/*",  MetaTestRunner::Expected::ToSucceed );
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    SECTION(    "selftest/test counts", 
                "Number of test cases that run is fixed" ) {
        EmbeddedRunner runner;
        
        SECTION(    "selftest/test counts/succeeding tests", 
                    "Number of 'succeeding' tests is fixed" ) {
            Totals totals = runner.runMatching( "./succeeding/*" );
            CHECK( totals.assertions.passed == 285 );
            CHECK( totals.assertions.failed == 0 );
        }

        SECTION(    "selftest/test counts/failing tests", 
                    "Number of 'failing' tests is fixed" ) {
            Totals totals = runner.runMatching( "./failing/*" );        
            CHECK( totals.assertions.passed == 0 );
            CHECK( totals.assertions.failed == 72 );
        }
    }
}

TEST_CASE( "meta/Misc/Sections", "looped tests" ) {
    Catch::EmbeddedRunner runner;
    
    Catch::Totals totals = runner.runMatching( "./mixed/Misc/Sections/nested2" );
    CHECK( totals.assertions.passed == 2 );
    CHECK( totals.assertions.failed == 1 );
}

#ifdef __clang__
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif

#include "../../include/internal/catch_commandline.hpp"
#include "../../include/reporters/catch_reporter_basic.hpp"
#include "../../include/reporters/catch_reporter_xml.hpp"
#include "../../include/reporters/catch_reporter_junit.hpp"

template<size_t size>
void parseIntoConfig( const char * (&argv)[size], Catch::ConfigData& config ) {
    Catch::parseIntoConfig( Catch::CommandParser( size, argv ), config );
}

template<size_t size>
std::string parseIntoConfigAndReturnError( const char * (&argv)[size], Catch::ConfigData& config ) {
    try {
        Catch::parseIntoConfig( Catch::CommandParser( size, argv ), config );
        FAIL( "expected exception" );
    }
    catch( std::exception& ex ) {
        return ex.what();
    }
    return "";
}

TEST_CASE( "selftest/parser/2", "ConfigData" ) {

    Catch::ConfigData config;

    SECTION( "default", "" ) {
        const char* argv[] = { "test" };
        CHECK_NOTHROW( parseIntoConfig( argv, config ) );
        
        CHECK( config.testSpecs.empty() );
        CHECK( config.shouldDebugBreak == false );
        CHECK( config.cutoff == -1 );
        CHECK( config.allowThrows == true );
        CHECK( config.reporter.empty() );
    }
    
    SECTION( "test lists", "" ) {
        SECTION( "-t/1", "Specify one test case using -t" ) {
            const char* argv[] = { "test", "-t", "test1" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.testSpecs.size() == 1 );
            REQUIRE( config.testSpecs[0] == "test1" );
        }

        SECTION( "--test/1", "Specify one test case using --test" ) {
            const char* argv[] = { "test", "--test", "test1" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.testSpecs.size() == 1 );
            REQUIRE( config.testSpecs[0] == "test1" );
        }

        SECTION( "-t/2", "Specify two test cases using -t" ) {
            const char* argv[] = { "test", "-t", "test1", "test2" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );

            REQUIRE( config.testSpecs.size() == 2 );
            REQUIRE( config.testSpecs[0] == "test1" );
            REQUIRE( config.testSpecs[1] == "test2" );
        }

        SECTION( "-t/0", "When no test names are supplied it is an error" ) {
            const char* argv[] = { "test", "-t" };
            REQUIRE_THAT( parseIntoConfigAndReturnError( argv, config ), Contains( "at least one" ) );
        }
    }
    
    SECTION( "reporter", "" ) {
        SECTION( "-r/basic", "" ) {
            const char* argv[] = { "test", "-r", "basic" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.reporter == "basic" );
        }
        SECTION( "-r/xml", "" ) {
            const char* argv[] = { "test", "-r", "xml" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.reporter == "xml" );
        }
        SECTION( "--reporter/junit", "" ) {
            const char* argv[] = { "test", "--reporter", "junit" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.reporter == "junit" );
        }
        SECTION( "-r/error", "reporter config only accepts one argument" ) {
            const char* argv[] = { "test", "-r", "one", "two" };
            REQUIRE_THAT( parseIntoConfigAndReturnError( argv, config ), Contains( "one argument" ) );
        }
    }
    
    SECTION( "debugger", "" ) {
        SECTION( "-b", "" ) {
            const char* argv[] = { "test", "-b" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.shouldDebugBreak == true );
        }
        SECTION( "--break", "" ) {
            const char* argv[] = { "test", "--break" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.shouldDebugBreak );
        }
        SECTION( "-b", "break option has no arguments" ) {
            const char* argv[] = { "test", "-b", "unexpected" };
            REQUIRE_THAT( parseIntoConfigAndReturnError( argv, config ), Contains( "not accept" ) );
        }
    }
        
    SECTION( "abort", "" ) {
        SECTION( "-a", "" ) {
            const char* argv[] = { "test", "-a" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );

            REQUIRE( config.cutoff == 1 );
        }
        SECTION( "-a/2", "" ) {
            const char* argv[] = { "test", "-a", "2" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );

            REQUIRE( config.cutoff == 2 );
        }
        SECTION( "-a/error/0", "" ) {
            const char* argv[] = { "test", "-a", "0" };
            REQUIRE_THAT( parseIntoConfigAndReturnError( argv, config ), Contains( "greater than zero" ) );
        }
        SECTION( "-a/error/non numeric", "" ) {
            const char* argv[] = { "test", "-a", "oops" };
            REQUIRE_THAT( parseIntoConfigAndReturnError( argv, config ), Contains( "greater than zero" ) );
        }
        SECTION( "-a/error/two args", "cutoff only takes one argument" ) {
            const char* argv[] = { "test", "-a", "1", "2" };
            REQUIRE_THAT( parseIntoConfigAndReturnError( argv, config ), Contains( "accepts" ) );
        }
    }
    
    SECTION( "nothrow", "" ) {
        SECTION( "-nt", "" ) {
            const char* argv[] = { "test", "-nt" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );

            REQUIRE( config.allowThrows == false );
        }
        SECTION( "--nothrow", "" ) {
            const char* argv[] = { "test", "--nothrow" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );

            REQUIRE( config.allowThrows == false );
        }
    
    }
    SECTION( "combinations", "" ) {
        SECTION( "-a -b", "" ) {
            const char* argv[] = { "test", "-a", "-b", "-nt" };
            CHECK_NOTHROW( parseIntoConfig( argv, config ) );

            CHECK( config.cutoff == 1 );
            CHECK( config.shouldDebugBreak );
            CHECK( config.allowThrows == false );
        }
    }        
}
