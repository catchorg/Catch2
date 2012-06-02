/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
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
            runner.runMatching( "./succeeding/*" );
            CHECK( runner.getTotals().assertions.passed == 277 );
            CHECK( runner.getTotals().assertions.failed == 0 );
        }

        SECTION(    "selftest/test counts/failing tests", 
                    "Number of 'failing' tests is fixed" ) {
            runner.runMatching( "./failing/*" );        
            CHECK( runner.getTotals().assertions.passed == 0 );
            CHECK( runner.getTotals().assertions.failed == 72 );
        }
    }
}

TEST_CASE( "meta/Misc/Sections", "looped tests" ) {
    Catch::EmbeddedRunner runner;
    
    runner.runMatching( "./mixed/Misc/Sections/nested2" );
    CHECK( runner.getTotals().assertions.passed == 2 );
    CHECK( runner.getTotals().assertions.failed == 1 );
}

#include "../../include/internal/catch_commandline.hpp"
#include "../../include/reporters/catch_reporter_basic.hpp"
#include "../../include/reporters/catch_reporter_xml.hpp"
#include "../../include/reporters/catch_reporter_junit.hpp"

template<size_t size>
bool parseIntoConfig( const char * (&argv)[size], Catch::Config& config ) {
    return Catch::parseIntoConfig( Catch::CommandParser( size, argv ), config );
}

TEST_CASE( "selftest/parser", "" ) {

    SECTION( "default", "" ) {
        const char* argv[] = { "test" };
        Catch::Config config;
        CHECK( parseIntoConfig( argv, config ) );
        
        CHECK( config.getTestSpecs().empty() );
        CHECK( config.shouldDebugBreak() == false );
        CHECK( config.showHelp() == false );
        CHECK( config.getCutoff() == -1 );
        CHECK( dynamic_cast<Catch::BasicReporter*>( config.getReporter().get() ) );
    }
    
    SECTION( "test lists", "" ) {
        SECTION( "-t/1", "Specify one test case using -t" ) {
            const char* argv[] = { "test", "-t", "test1" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.getTestSpecs().size() == 1 );
            REQUIRE( config.getTestSpecs()[0] == "test1" );
        }

        SECTION( "--test/1", "Specify one test case using --test" ) {
            const char* argv[] = { "test", "--test", "test1" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.getTestSpecs().size() == 1 );
            REQUIRE( config.getTestSpecs()[0] == "test1" );
        }

        SECTION( "-t/2", "Specify two test cases using -t" ) {
            const char* argv[] = { "test", "-t", "test1", "test2" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );

            REQUIRE( config.getTestSpecs().size() == 2 );
            REQUIRE( config.getTestSpecs()[0] == "test1" );
            REQUIRE( config.getTestSpecs()[1] == "test2" );
        }

        SECTION( "-t/0", "When no test names are supplied it is an error" ) {
            const char* argv[] = { "test", "-t" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) == false );
            
            REQUIRE_THAT( config.getMessage(), Contains( "none" ) );
        }
    }
    
    SECTION( "reporter", "" ) {
        SECTION( "-r/basic", "" ) {
            const char* argv[] = { "test", "-reporter", "basic" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );
            
            REQUIRE( dynamic_cast<Catch::BasicReporter*>( config.getReporter().get() ) );
        }
        SECTION( "-r/xml", "" ) {
            const char* argv[] = { "test", "-r", "xml" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );
            
            REQUIRE( dynamic_cast<Catch::XmlReporter*>( config.getReporter().get() ) );
        }
        SECTION( "-r/junit", "" ) {
            const char* argv[] = { "test", "-r", "junit" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );
            
            REQUIRE( dynamic_cast<Catch::JunitReporter*>( config.getReporter().get() ) );
        }
        SECTION( "-r/error", "reporter config only accepts one argument" ) {
            const char* argv[] = { "test", "-r", "one", "two" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) == false );
            
            REQUIRE_THAT( config.getMessage(), Contains( "one argument" ) );
        }
    }
    
    SECTION( "debugger", "" ) {
        SECTION( "-b", "" ) {
            const char* argv[] = { "test", "-b" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.shouldDebugBreak() == true );
        }
        SECTION( "--break", "" ) {
            const char* argv[] = { "test", "--break" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );
            
            REQUIRE( config.shouldDebugBreak() );
        }
        SECTION( "-b", "break option has no arguments" ) {
            const char* argv[] = { "test", "-b", "unexpected" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) == false );
            
            REQUIRE_THAT( config.getMessage(), Contains( "not accept" ) );
        }
    }
    
    SECTION( "help", "" ) {
        SECTION( "-h", "" ) {
            const char* argv[] = { "test", "-h" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );

            REQUIRE( config.showHelp() );
        }
        SECTION( "-?", "" ) {
            const char* argv[] = { "test", "-?" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );

            REQUIRE( config.showHelp() );
        }
        SECTION( "--help", "" ) {
            const char* argv[] = { "test", "--help" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );

            REQUIRE( config.showHelp() );
        }
        SECTION( "-h", "help option has no arguments" ) {
            const char* argv[] = { "test", "-h", "unexpected" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) == false );
            
            REQUIRE_THAT( config.getMessage(), Contains( "not accept" ) );
        }
    }
    
    SECTION( "cutoff", "" ) {
        SECTION( "-c", "" ) {
            const char* argv[] = { "test", "-c" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );

            REQUIRE( config.getCutoff() == 1 );
        }
        SECTION( "-c/2", "" ) {
            const char* argv[] = { "test", "-c", "2" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );

            REQUIRE( config.getCutoff() == 2 );
        }
        SECTION( "-c/error", "cutoff only takes one argument" ) {
            const char* argv[] = { "test", "-c", "1", "2" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) == false );

            REQUIRE_THAT( config.getMessage(), Contains( "accepts" ) );
        }
    }
    SECTION( "combinations", "" ) {
        SECTION( "-c -b", "" ) {
            const char* argv[] = { "test", "-c", "-b" };
            Catch::Config config;
            CHECK( parseIntoConfig( argv, config ) );

            REQUIRE( config.getCutoff() == 1 );
            REQUIRE( config.shouldDebugBreak() );
        }
    }
    
}
