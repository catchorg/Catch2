/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "internal/clara.h"

#include "catch.hpp"


// Helper to deduce size from array literals and pass on to parser
template<size_t size, typename ConfigT>
std::vector<Clara::Parser::Token> parseInto( Clara::CommandLine<ConfigT>& cli, char const* (&argv)[size], ConfigT& config ) {
    return cli.parseInto( size, argv, config );
}


struct TestOpt {
    TestOpt() : number( 0 ), index( 0 ), flag( false ) {}

    std::string fileName;
    int number;
    int index;
    bool flag;
    std::string firstPos;
    std::string secondPos;
    std::string unpositional;
    
    void setValidIndex( int i ) {
        if( i < 0 || i > 10 )
            throw std::domain_error( "index must be between 0 and 10" );
        index = i;
    }
};

struct TestOpt2 {
    std::string description;
};

#ifdef CATCH_CONFIG_VARIADIC_MACROS

TEST_CASE( "cmdline" ) {

    TestOpt config;
    Clara::CommandLine<TestOpt> cli;
    cli.bind( &TestOpt::fileName )
        .describe( "specifies output file" )
        .shortOpt( "o" )
        .longOpt( "output" )
        .argName( "filename" );

    SECTION( "arg separated by spaces" ) {
        const char* argv[] = { "test", "-o filename.ext" };
        parseInto( cli, argv, config );
        
        CHECK( config.fileName == "filename.ext" );
    }
    SECTION( "arg separated by colon" ) {
        const char* argv[] = { "test", "-o:filename.ext" };
        parseInto( cli, argv, config );

        CHECK( config.fileName == "filename.ext" );
    }
    SECTION( "arg separated by =" ) {
        const char* argv[] = { "test", "-o=filename.ext" };
        parseInto( cli, argv, config );

        CHECK( config.fileName == "filename.ext" );
    }
    SECTION( "long opt" ) {
        const char* argv[] = { "test", "--output %stdout" };
        parseInto( cli, argv, config );

        CHECK( config.fileName == "%stdout" );
    }
    
    cli.bind( &TestOpt::number )
            .shortOpt( "n" )
            .argName( "an integral value" );
    
    SECTION( "a number" ) {
        const char* argv[] = { "test", "-n 42" };
        parseInto( cli, argv, config );

        CHECK( config.number == 42 );
    }
    SECTION( "not a number" ) {
        const char* argv[] = { "test", "-n forty-two" };
        CHECK_THROWS( parseInto( cli, argv, config ) );

        CHECK( config.number == 0 );
    }
    
    SECTION( "two parsers" ) {

        TestOpt config1;
        TestOpt2 config2;
        Clara::CommandLine<TestOpt2> cli2;

        cli2.bind( &TestOpt2::description )
            .describe( "description" )
            .shortOpt( "d" )
            .longOpt( "description" )
            .argName( "some text" );
        
        const char* argv[] = { "test", "-n 42", "-d some text" };
        std::vector<Clara::Parser::Token> unusedTokens = parseInto( cli, argv, config1 );

        CHECK( config1.number == 42 );

        REQUIRE_FALSE( unusedTokens.empty() );
        cli2.populate( unusedTokens, config2 );
        CHECK( config2.description == "some text" );        
    }

    SECTION( "methods" ) {
        cli.bind( &TestOpt::setValidIndex )
            .describe( "An index, which is an integer between 0 and 10, inclusive" )
            .shortOpt( "i" )
            .argName( "index" );

        SECTION( "in range" ) {
            const char* argv[] = { "test", "-i 3" };
            parseInto( cli, argv, config );

            REQUIRE( config.index == 3 );
        }
        SECTION( "out of range" ) {
            const char* argv[] = { "test", "-i 42" };

            REQUIRE_THROWS( parseInto( cli, argv, config ) );
        }
    }
    
    SECTION( "flags" ) {
        cli.bind( &TestOpt::flag )
            .describe( "A flag" )
            .shortOpt( "f" );
        
        SECTION( "set" ) {
            const char* argv[] = { "test", "-f" };
            parseInto( cli, argv, config );

            REQUIRE( config.flag );
        }
        SECTION( "not set" ) {
            const char* argv[] = { "test" };
            parseInto( cli, argv, config );

            REQUIRE( config.flag == false );
        }
    }
    SECTION( "positional" ) {
        cli.bind( &TestOpt::secondPos )
            .describe( "Second position" )
            .argName( "second arg" )
            .position( 2 );
        cli.bind( &TestOpt::unpositional )
            .argName( "any arg" )
            .describe( "Unpositional" );
        cli.bind( &TestOpt::firstPos )
            .describe( "First position" )
            .argName( "first arg" )
            .position( 1 );

        std::cout << cli.usage( "testApp" ) << std::endl;

        const char* argv[] = { "test", "-f", "1st", "-o", "filename", "2nd", "3rd" };
        parseInto( cli, argv, config );

        REQUIRE( config.firstPos == "1st" );
        REQUIRE( config.secondPos == "2nd" );
        REQUIRE( config.unpositional == "3rd" );
    }
}

struct Config {

    struct Verbosity { enum Level { NoOutput = 0, Quiet, Normal }; };
    struct Warning { enum Types { Nothing = 0x00, NoAssertions = 0x01 }; };

    Config()
    :   listTests( false ),
        listTags( false ),
        showSuccessfulTests( false ),
        breakIntoDebugger( false ),
        noThrow( false ),
        showHelp( false ),
        abortAfter( 0 ),
        verbosity( Verbosity::Normal )
    {}

    bool listTests;
    bool listTags;
    bool listReporters;

    bool showSuccessfulTests;
    bool breakIntoDebugger;
    bool noThrow;
    bool showHelp;

    int abortAfter;

    Verbosity::Level verbosity;
    Warning::Types warnings;

    std::string reporterName;
    std::string fileName;
    std::string suiteName;
    std::string reporter;

    std::vector<std::string> testsOrTags;
};

inline void abortAfterFirst( Config& config ) { config.abortAfter = 1; }
inline void abortAfterX( Config& config, int x ) { config.abortAfter = x; }
inline void addTestOrTags( Config& config, std::string const& _testSpec ) { config.testsOrTags.push_back( _testSpec ); }

inline void addWarning( Config& config, std::string const& _warning ) {
    if( _warning == "NoAssertions" )
        config.warnings = (Config::Warning::Types)( config.warnings | Config::Warning::NoAssertions );
    else
        throw std::runtime_error( "Unrecognised warning: '" + _warning + "'" );

}
inline void setVerbosity( Config& config, int level ) {
    // !TBD: accept strings?
    config.verbosity = (Config::Verbosity::Level)level;
}


SCENARIO( "New Catch commandline interface", "[cli]" ) {

    GIVEN( "A built cli parser for Catch" ) {
        Clara::CommandLine<Config> cli;

        cli.bind( &Config::showHelp )
            .describe( "display usage information" )
            .shortOpt( "?")
            .shortOpt( "h")
            .longOpt( "help" );

        cli.bind( &Config::listTests )
            .describe( "list all (or matching) test cases" )
            .shortOpt( "l")
            .longOpt( "list-tests" );

        cli.bind( &Config::listTags )
            .describe( "list all (or matching) tags" )
            .shortOpt( "t")
            .longOpt( "list-tags" );

        cli.bind( &Config::listTags )
            .describe( "list all reporters" )
            .longOpt( "list-reporters" );

        cli.bind( &Config::showSuccessfulTests )
            .describe( "include successful tests in output" )
            .shortOpt( "s")
            .longOpt( "success" );

        cli.bind( &Config::breakIntoDebugger )
            .describe( "break into debugger on failure" )
            .shortOpt( "b")
            .longOpt( "break" );

        cli.bind( &Config::noThrow )
            .describe( "Skip exception tests" )
            .shortOpt( "e")
            .longOpt( "nothrow" );

        cli.bind( &Config::fileName )
            .describe( "output filename" )
            .shortOpt( "o")
            .longOpt( "out" )
            .argName( "filename" );

        cli.bind( &Config::reporter )
            .describe( "reporter to use - defaults to console" )
            .shortOpt( "r")
            .longOpt( "reporter" )
            .argName( "name[:filename]" );

        cli.bind( &Config::suiteName )
            .describe( "suite name" )
            .shortOpt( "n")
            .longOpt( "name" )
            .argName( "name" );

        cli.bind( &abortAfterFirst )
            .describe( "abort at first failure" )
            .shortOpt( "a")
            .longOpt( "abort" );

        cli.bind( &abortAfterX )
            .describe( "abort after x failures" )
            .shortOpt( "x")
            .longOpt( "abortx" )
            .argName( "number of failures" );

        cli.bind( &addWarning )
            .describe( "enable warnings" )
            .shortOpt( "w")
            .longOpt( "warn" )
            .argName( "warning name" );

        cli.bind( &setVerbosity )
            .describe( "level of verbosity (0=no output)" )
            .shortOpt( "v")
            .longOpt( "verbosity" )
            .argName( "level" );

        cli.bind( &addTestOrTags )
            .describe( "which test or tests to use" )
            .argName( "test name, pattern or tags" );

        WHEN( "We ask for usage strings" )
        THEN( "It prints the usage strings" )
            std::cout << cli.usage( "CatchTestApp" ) << std::endl;
        
        Config config;

        WHEN( "Multiple flags are combined" ) {

            CHECK_FALSE( config.showSuccessfulTests );
            CHECK_FALSE( config.noThrow );
            CHECK_FALSE( config.breakIntoDebugger );

            const char* argv[] = { "test", "-seb" };
            parseInto( cli, argv, config );
            
            THEN( "All the flags are set" ) {
                CHECK( config.showSuccessfulTests );
                CHECK( config.noThrow );
                CHECK( config.breakIntoDebugger );
            }
        }
        WHEN( "A flag is set via a nullary method" ) {
            CHECK( config.abortAfter == 0 );

            const char* argv[] = { "test", "-a" };
            parseInto( cli, argv, config );
            
            THEN( "The flag is set" )
                REQUIRE( config.abortAfter == 1 );
        }
        WHEN( "A flag is set via a unary method" ) {
            CHECK( config.abortAfter == 0 );

            const char* argv[] = { "test", "-x", "2" };
            parseInto( cli, argv, config );
            
            THEN( "The flag is set" )
                REQUIRE( config.abortAfter == 2 );
        }
        WHEN( "A positional argument is supplied" ) {

            const char* argv[] = { "test", "[hello]" };
            parseInto( cli, argv, config );
            
            THEN( "The argument is in the testOrTags collection" ) {
                REQUIRE( config.testsOrTags.size() == 1 );
                REQUIRE( config.testsOrTags[0] == "[hello]" );
            }
        }
        WHEN( "And enum opt is set by numeric value" ) {
            CHECK( config.verbosity == Config::Verbosity::Normal );

            const char* argv[] = { "test", "-v 0" };
            parseInto( cli, argv, config );
            
            THEN( "The member is set to the enum value" )
                REQUIRE( config.verbosity == Config::Verbosity::NoOutput );
        }
    }
}
#endif
