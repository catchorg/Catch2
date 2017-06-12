/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/reporters/catch_reporter_teamcity.hpp"
#include "../include/reporters/catch_reporter_tap.hpp"
#include "../include/reporters/catch_reporter_automake.hpp"


// Some example tag aliases
CATCH_REGISTER_TAG_ALIAS( "[@nhf]", "[failing]~[.]" )
CATCH_REGISTER_TAG_ALIAS( "[@tricky]", "[tricky]~[.]" )


#ifdef __clang__
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wweak-vtables"
#   pragma clang diagnostic ignored "-Wc++98-compat"
#endif


//template<size_t size>
//void parseIntoConfig( const char * (&argv)[size], Catch::ConfigData& config ) {
//    auto parser = Catch::makeCommandLineParser();
//    parser.parseInto( Catch::Clara::argsToVector( size, argv ), config );
//}
//
//template<size_t size>
//std::string parseIntoConfigAndReturnError( const char * (&argv)[size], Catch::ConfigData& config ) {
//    try {
//        parseIntoConfig( argv, config );
//        FAIL( "expected exception" );
//    }
//    catch( std::exception& ex ) {
//        return ex.what();
//    }
//    return "";
//}
//
inline Catch::TestCase fakeTestCase( const char* name, const char* desc = "" ){ return Catch::makeTestCase( nullptr, "", name, desc, CATCH_INTERNAL_LINEINFO ); }

TEST_CASE( "Process can be configured on command line", "[config][command-line]" ) {

    using namespace Catch::Matchers;

    Catch::ConfigData config;
    auto cli = Catch::makeCommandLineParser(config);

    SECTION("empty args don't cause a crash") {
        auto result = cli.parse({""});
        CHECK(result);
        CHECK(config.processName == "");
    }


    SECTION("default - no arguments") {
        auto result = cli.parse({"test"});
        CHECK(result);
        CHECK(config.processName == "test");
        CHECK(config.shouldDebugBreak == false);
        CHECK(config.abortAfter == -1);
        CHECK(config.noThrow == false);
        CHECK(config.reporterNames.empty());
    }

    SECTION("test lists") {
        SECTION("1 test", "Specify one test case using") {
            auto result = cli.parse({"test", "test1"});
            CHECK(result);

            Catch::Config cfg(config);
            REQUIRE(cfg.testSpec().matches(fakeTestCase("notIncluded")) == false);
            REQUIRE(cfg.testSpec().matches(fakeTestCase("test1")));
        }
        SECTION("Specify one test case exclusion using exclude:") {
            auto result = cli.parse({"test", "exclude:test1"});
            CHECK(result);

            Catch::Config cfg(config);
            REQUIRE(cfg.testSpec().matches(fakeTestCase("test1")) == false);
            REQUIRE(cfg.testSpec().matches(fakeTestCase("alwaysIncluded")));
        }

        SECTION("Specify one test case exclusion using ~") {
            auto result = cli.parse({"test", "~test1"});
            CHECK(result);

            Catch::Config cfg(config);
            REQUIRE(cfg.testSpec().matches(fakeTestCase("test1")) == false);
            REQUIRE(cfg.testSpec().matches(fakeTestCase("alwaysIncluded")));
        }

    }

    SECTION("reporter") {
        SECTION("-r/console") {
            CHECK(cli.parse({"test", "-r", "console"}));

            REQUIRE(config.reporterNames[0] == "console");
        }
        SECTION("-r/xml") {
            CHECK(cli.parse({"test", "-r", "xml"}));

            REQUIRE(config.reporterNames[0] == "xml");
        }
        SECTION("-r xml and junit") {
            CHECK(cli.parse({"test", "-r", "xml", "-r", "junit"}));

            REQUIRE(config.reporterNames.size() == 2);
            REQUIRE(config.reporterNames[0] == "xml");
            REQUIRE(config.reporterNames[1] == "junit");
        }
        SECTION("--reporter/junit") {
            CHECK(cli.parse({"test", "--reporter", "junit"}));

            REQUIRE(config.reporterNames[0] == "junit");
        }
    }


    SECTION("debugger") {
        SECTION("-b") {
            CHECK(cli.parse({"test", "-b"}));

            REQUIRE(config.shouldDebugBreak == true);
        }
        SECTION("--break") {
            CHECK(cli.parse({"test", "--break"}));

            REQUIRE(config.shouldDebugBreak);
        }
    }


    SECTION("abort") {
        SECTION("-a aborts after first failure") {
            CHECK(cli.parse({"test", "-a"}));

            REQUIRE(config.abortAfter == 1);
        }
        SECTION("-x 2 aborts after two failures") {
            CHECK(cli.parse({"test", "-x", "2"}));

            REQUIRE(config.abortAfter == 2);
        }
        SECTION("-x must be numeric") {
            auto result = cli.parse({"test", "-x", "oops"});
            CHECK(!result);

            REQUIRE_THAT(result.errorMessage(), Contains("convert") && Contains("oops"));
        }
    }

    SECTION("nothrow") {
        SECTION("-e") {
            CHECK(cli.parse({"test", "-e"}));

            REQUIRE(config.noThrow);
        }
        SECTION("--nothrow") {
            CHECK(cli.parse({"test", "--nothrow"}));

            REQUIRE(config.noThrow);
        }
    }

    SECTION("output filename") {
        SECTION("-o filename") {
            CHECK(cli.parse({"test", "-o", "filename.ext"}));

            REQUIRE(config.outputFilename == "filename.ext");
        }
        SECTION("--out") {
            CHECK(cli.parse({"test", "--out", "filename.ext"}));

            REQUIRE(config.outputFilename == "filename.ext");
        }
    }

    SECTION("combinations") {
        SECTION("Single character flags can be combined") {
            CHECK(cli.parse({"test", "-abe"}));

            CHECK(config.abortAfter == 1);
            CHECK(config.shouldDebugBreak);
            CHECK(config.noThrow == true);
        }
    }


    SECTION( "use-colour") {

        using Catch::UseColour;

        SECTION( "without option" ) {
            CHECK(cli.parse({"test"}));

            REQUIRE( config.useColour == UseColour::Auto );
        }

        SECTION( "auto" ) {
            CHECK(cli.parse({"test", "--use-colour", "auto"}));

            REQUIRE( config.useColour == UseColour::Auto );
        }

        SECTION( "yes" ) {
            CHECK(cli.parse({"test", "--use-colour", "yes"}));

            REQUIRE( config.useColour == UseColour::Yes );
        }

        SECTION( "no" ) {
            CHECK(cli.parse({"test", "--use-colour", "no"}));

            REQUIRE( config.useColour == UseColour::No );
        }

        SECTION( "error" ) {
            auto result = cli.parse({"test", "--use-colour", "wrong"});
            CHECK( !result );
            CHECK_THAT( result.errorMessage(), Contains( "colour mode must be one of" ) );
        }
    }
}


TEST_CASE( "Long strings can be wrapped", "[wrap]" ) {

    using namespace Catch;
    SECTION( "plain string" ) {
        // guide:                 123456789012345678
        std::string testString = "one two three four";

        SECTION( "No wrapping" ) {
            CHECK( Text( testString, TextAttributes().setWidth( 80 ) ).toString() == testString );
            CHECK( Text( testString, TextAttributes().setWidth( 18 ) ).toString() == testString );
        }
        SECTION( "Wrapped once" ) {
            CHECK( Text( testString, TextAttributes().setWidth( 17 ) ).toString() == "one two three\nfour" );
            CHECK( Text( testString, TextAttributes().setWidth( 16 ) ).toString() == "one two three\nfour" );
            CHECK( Text( testString, TextAttributes().setWidth( 14 ) ).toString() == "one two three\nfour" );
            CHECK( Text( testString, TextAttributes().setWidth( 13 ) ).toString() == "one two three\nfour" );
            CHECK( Text( testString, TextAttributes().setWidth( 12 ) ).toString() == "one two\nthree four" );
        }
        SECTION( "Wrapped twice" ) {
            CHECK( Text( testString, TextAttributes().setWidth( 9 ) ).toString() == "one two\nthree\nfour" );
            CHECK( Text( testString, TextAttributes().setWidth( 8 ) ).toString() == "one two\nthree\nfour" );
            CHECK( Text( testString, TextAttributes().setWidth( 7 ) ).toString() == "one two\nthree\nfour" );
        }
        SECTION( "Wrapped three times" ) {
            CHECK( Text( testString, TextAttributes().setWidth( 6 ) ).toString() == "one\ntwo\nthree\nfour" );
            CHECK( Text( testString, TextAttributes().setWidth( 5 ) ).toString() == "one\ntwo\nthree\nfour" );
        }
        SECTION( "Short wrap" ) {
            CHECK( Text( "abcdef", TextAttributes().setWidth( 4 ) ).toString() == "abc-\ndef" );
            CHECK( Text( "abcdefg", TextAttributes().setWidth( 4 ) ).toString() == "abc-\ndefg" );
            CHECK( Text( "abcdefgh", TextAttributes().setWidth( 4 ) ).toString() == "abc-\ndef-\ngh" );

            CHECK( Text( testString, TextAttributes().setWidth( 4 ) ).toString() == "one\ntwo\nthr-\nee\nfour" );
            CHECK( Text( testString, TextAttributes().setWidth( 3 ) ).toString() == "one\ntwo\nth-\nree\nfo-\nur" );
        }
        SECTION( "As container" ) {
            Text text( testString, TextAttributes().setWidth( 6 ) );
            REQUIRE( text.size() == 4 );
            CHECK( text[0] == "one" );
            CHECK( text[1] == "two" );
            CHECK( text[2] == "three" );
            CHECK( text[3] == "four" );
        }
        SECTION( "Indent first line differently" ) {
            Text text( testString, TextAttributes()
                                        .setWidth( 10 )
                                        .setIndent( 4 )
                                        .setInitialIndent( 1 ) );
            CHECK( text.toString() == " one two\n    three\n    four" );
        }

    }

    SECTION( "With newlines" ) {

        // guide:                 1234567890123456789
        std::string testString = "one two\nthree four";

        SECTION( "No wrapping" ) {
            CHECK( Text( testString, TextAttributes().setWidth( 80 ) ).toString() == testString );
            CHECK( Text( testString, TextAttributes().setWidth( 18 ) ).toString() == testString );
            CHECK( Text( testString, TextAttributes().setWidth( 10 ) ).toString() == testString );
        }
        SECTION( "Trailing newline" ) {
            CHECK( Text( "abcdef\n", TextAttributes().setWidth( 10 ) ).toString() == "abcdef" );
            CHECK( Text( "abcdef", TextAttributes().setWidth( 6 ) ).toString() == "abcdef" );
            CHECK( Text( "abcdef\n", TextAttributes().setWidth( 6 ) ).toString() == "abcdef" );
            CHECK( Text( "abcdef\n", TextAttributes().setWidth( 5 ) ).toString() == "abcd-\nef" );
        }
        SECTION( "Wrapped once" ) {
            CHECK( Text( testString, TextAttributes().setWidth( 9 ) ).toString() == "one two\nthree\nfour" );
            CHECK( Text( testString, TextAttributes().setWidth( 8 ) ).toString() == "one two\nthree\nfour" );
            CHECK( Text( testString, TextAttributes().setWidth( 7 ) ).toString() == "one two\nthree\nfour" );
        }
        SECTION( "Wrapped twice" ) {
            CHECK( Text( testString, TextAttributes().setWidth( 6 ) ).toString() == "one\ntwo\nthree\nfour" );
        }
    }

    SECTION( "With wrap-before/ after characters" ) {
        std::string testString = "one,two(three) <here>";

        SECTION( "No wrapping" ) {
            CHECK( Text( testString, TextAttributes().setWidth( 80 ) ).toString() == testString );
            CHECK( Text( testString, TextAttributes().setWidth( 24 ) ).toString() == testString );
        }
        SECTION( "Wrap before" ) {
            CHECK( Text( testString, TextAttributes().setWidth( 11 ) ).toString() == "one,two\n(three)\n<here>" );
        }
        SECTION( "Wrap after" ) {
            CHECK( Text( testString, TextAttributes().setWidth( 6 ) ).toString() == "one,\ntwo\n(thre-\ne)\n<here>" );
            CHECK( Text( testString, TextAttributes().setWidth( 5 ) ).toString() == "one,\ntwo\n(thr-\nee)\n<her-\ne>" );
            CHECK( Text( testString, TextAttributes().setWidth( 4 ) ).toString() == "one,\ntwo\n(th-\nree)\n<he-\nre>" );
        }
    }

}

using namespace Catch;

class ColourString {
public:

    struct ColourIndex {
        ColourIndex( Colour::Code _colour, std::size_t _fromIndex, std::size_t _toIndex )
        :   colour( _colour ),
            fromIndex( _fromIndex ),
            toIndex( _toIndex )
        {}

        Colour::Code colour;
        std::size_t fromIndex;
        std::size_t toIndex;
    };

    ColourString( std::string const& _string )
    : string( _string )
    {}
    ColourString( std::string const& _string, std::vector<ColourIndex> const& _colours )
    : string( _string ), colours( _colours )
    {}

    ColourString& addColour( Colour::Code colour, int _index ) {
        colours.push_back( ColourIndex( colour,
                                        resolveRelativeIndex( _index ),
                                        resolveRelativeIndex( _index )+1 ) );
        return *this;
    }
    ColourString& addColour( Colour::Code colour, int _fromIndex, int _toIndex ) {
        colours.push_back( ColourIndex( colour,
                                        resolveRelativeIndex(_fromIndex),
                                        resolveLastRelativeIndex( _toIndex ) ) );
        return *this;
    }

    void writeToStream( std::ostream& _stream ) const {
        std::size_t last = 0;
        for( std::size_t i = 0; i < colours.size(); ++i ) {
            ColourIndex const& index = colours[i];
            if( index.fromIndex > last )
                _stream << string.substr( last, index.fromIndex-last );
            {
                Colour colourGuard( index.colour );
                _stream << string.substr( index.fromIndex, index.toIndex-index.fromIndex );
            }
            last = index.toIndex;
        }
        if( last < string.size() )
            _stream << string.substr( last );
    }
    friend std::ostream& operator << ( std::ostream& _stream, ColourString const& _colourString ) {
        _colourString.writeToStream( _stream );
        return _stream;
    }

private:

    std::size_t resolveLastRelativeIndex( int _index ) {
        std::size_t index = resolveRelativeIndex( _index );
        return index == 0 ? string.size() : index;
    }
    std::size_t resolveRelativeIndex( int _index ) {
        return static_cast<std::size_t>( _index >= 0
            ? _index
            : static_cast<int>( string.size() )+_index );
    }
    std::string string;
    std::vector<ColourIndex> colours;
};

TEST_CASE( "replaceInPlace" ) {
    std::string letters = "abcdefcg";
    SECTION( "replace single char" ) {
        CHECK( replaceInPlace( letters, "b", "z" ) );
        CHECK( letters == "azcdefcg" );
    }
    SECTION( "replace two chars" ) {
        CHECK( replaceInPlace( letters, "c", "z" ) );
        CHECK( letters == "abzdefzg" );
    }
    SECTION( "replace first char" ) {
        CHECK( replaceInPlace( letters, "a", "z" ) );
        CHECK( letters == "zbcdefcg" );
    }
    SECTION( "replace last char" ) {
        CHECK( replaceInPlace( letters, "g", "z" ) );
        CHECK( letters == "abcdefcz" );
    }
    SECTION( "replace all chars" ) {
        CHECK( replaceInPlace( letters, letters, "replaced" ) );
        CHECK( letters == "replaced" );
    }
    SECTION( "replace no chars" ) {
        CHECK_FALSE( replaceInPlace( letters, "x", "z" ) );
        CHECK( letters == letters );
    }
    SECTION( "escape '" ) {
        std::string s = "didn't";
        CHECK( replaceInPlace( s, "'", "|'" ) );
        CHECK( s == "didn|'t" );
    }
}

// !TBD: This will be folded into Text class
TEST_CASE( "Strings can be rendered with colour", "[.colour]" ) {

    {
        ColourString cs( "hello" );
        cs  .addColour( Colour::Red, 0 )
            .addColour( Colour::Green, -1 );

        Catch::cout() << cs << std::endl;
    }

    {
        ColourString cs( "hello" );
        cs  .addColour( Colour::Blue, 1, -2 );

        Catch::cout() << cs << std::endl;
    }

}

TEST_CASE( "Text can be formatted using the Text class" ) {

    CHECK( Text( "hi there" ).toString() == "hi there" );

    TextAttributes narrow;
    narrow.setWidth( 6 );

    CHECK( Text( "hi there", narrow ).toString() == "hi\nthere" );
}

TEST_CASE( "Long text is truncated", "[Text][Truncated]" ) {

    std::string longLine( 90, '*' );

    std::ostringstream oss;
    for(int i = 0; i < 600; ++i )
        oss << longLine << longLine << "\n";
    Text t( oss.str() );
    CHECK_THAT( t.toString(), EndsWith( "... message truncated due to excessive size" ) );

}

inline void manuallyRegisteredTestFunction() {
    SUCCEED( "was called" );
}
struct AutoTestReg {
    AutoTestReg() {
        REGISTER_TEST_CASE( manuallyRegisteredTestFunction, "ManuallyRegistered", "" );
    }
};
AutoTestReg autoTestReg;
