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

struct TestListener : Catch::TestEventListenerBase {
    using TestEventListenerBase::TestEventListenerBase; // inherit constructor
};
CATCH_REGISTER_LISTENER( TestListener );

inline Catch::TestCase fakeTestCase( const char* name, const char* desc = "" ){ return Catch::makeTestCase( nullptr, "", name, desc, CATCH_INTERNAL_LINEINFO ); }

TEST_CASE( "Process can be configured on command line", "[config][command-line]" ) {

#ifndef CATCH_CONFIG_DISABLE_MATCHERS
    using namespace Catch::Matchers;
#endif

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

#ifndef CATCH_CONFIG_DISABLE_MATCHERS
            REQUIRE_THAT(result.errorMessage(), Contains("convert") && Contains("oops"));
#endif
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
#ifndef CATCH_CONFIG_DISABLE_MATCHERS
            CHECK_THAT( result.errorMessage(), Contains( "colour mode must be one of" ) );
#endif
        }
    }
}


TEST_CASE( "replaceInPlace" ) {
    std::string letters = "abcdefcg";
    SECTION( "replace single char" ) {
        CHECK( Catch::replaceInPlace( letters, "b", "z" ) );
        CHECK( letters == "azcdefcg" );
    }
    SECTION( "replace two chars" ) {
        CHECK( Catch::replaceInPlace( letters, "c", "z" ) );
        CHECK( letters == "abzdefzg" );
    }
    SECTION( "replace first char" ) {
        CHECK( Catch::replaceInPlace( letters, "a", "z" ) );
        CHECK( letters == "zbcdefcg" );
    }
    SECTION( "replace last char" ) {
        CHECK( Catch::replaceInPlace( letters, "g", "z" ) );
        CHECK( letters == "abcdefcz" );
    }
    SECTION( "replace all chars" ) {
        CHECK( Catch::replaceInPlace( letters, letters, "replaced" ) );
        CHECK( letters == "replaced" );
    }
    SECTION( "replace no chars" ) {
        CHECK_FALSE( Catch::replaceInPlace( letters, "x", "z" ) );
        CHECK( letters == letters );
    }
    SECTION( "escape '" ) {
        std::string s = "didn't";
        CHECK( Catch::replaceInPlace( s, "'", "|'" ) );
        CHECK( s == "didn|'t" );
    }
}


inline void manuallyRegisteredTestFunction() {
    SUCCEED( "was called" );
}
struct AutoTestReg {
    AutoTestReg() {
        REGISTER_TEST_CASE( manuallyRegisteredTestFunction, "ManuallyRegistered" );
    }
};
static AutoTestReg autoTestReg;
