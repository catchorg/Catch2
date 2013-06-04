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
std::vector<Clara::Parser::Token> parseInto( Clara::CommandLine<ConfigT>& cli, char const * (&argv)[size], ConfigT& config ) {
    return cli.parseInto( size, argv, config );
}


struct TestOpt {
    TestOpt() : number( 0 ), index( 0 ), flag( false ) {}

    std::string processName;
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
    cli.bindProcessName( &TestOpt::processName );
    cli.bind( &TestOpt::fileName )
        .describe( "specifies output file" )
        .shortOpt( "o" )
        .longOpt( "output" )
        .argName( "filename" );

    SECTION( "process name" ) {
        char const * argv[] = { "test", "-o filename.ext" };
        parseInto( cli, argv, config );
        
        CHECK( config.processName == "test" );
    }
    SECTION( "arg separated by spaces" ) {
        char const * argv[] = { "test", "-o filename.ext" };
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

//        std::cout << cli.usage( "testApp" ) << std::endl;

        const char* argv[] = { "test", "-f", "1st", "-o", "filename", "2nd", "3rd" };
        parseInto( cli, argv, config );

        REQUIRE( config.firstPos == "1st" );
        REQUIRE( config.secondPos == "2nd" );
        REQUIRE( config.unpositional == "3rd" );
    }
}

#endif
