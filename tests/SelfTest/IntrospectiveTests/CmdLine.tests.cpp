
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_config.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/internal/catch_test_spec_parser.hpp>
#include <catch2/catch_user_config.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/internal/catch_commandline.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/internal/catch_compiler_capabilities.hpp>


namespace {
    auto fakeTestCase(const char* name, const char* desc = "") { return Catch::makeTestCaseInfo("", { name, desc }, CATCH_INTERNAL_LINEINFO); }
}

TEST_CASE( "Parse test names and tags", "[command-line][test-spec]" ) {

    using Catch::parseTestSpec;
    using Catch::TestSpec;

    auto tcA = fakeTestCase( "a" );
    auto tcB = fakeTestCase( "b", "[one][x]" );
    auto tcC = fakeTestCase( "longer name with spaces", "[two][three][.][x]" );
    auto tcD = fakeTestCase( "zlonger name with spacesz" );

    SECTION( "Empty test spec should have no filters" ) {
        TestSpec spec;
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
    }

    SECTION( "Test spec from empty string should have no filters" ) {
        TestSpec spec = parseTestSpec( "" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
    }

    SECTION( "Test spec from just a comma should have no filters" ) {
        TestSpec spec = parseTestSpec( "," );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
    }

    SECTION( "Test spec from name should have one filter" ) {
        TestSpec spec = parseTestSpec( "b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == true );
    }

    SECTION( "Test spec from quoted name should have one filter" ) {
        TestSpec spec = parseTestSpec( "\"b\"" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == true );
    }

    SECTION( "Test spec from name should have one filter" ) {
        TestSpec spec = parseTestSpec( "b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == true );
        CHECK( spec.matches( *tcC ) == false );
    }

    SECTION( "Wildcard at the start" ) {
        TestSpec spec = parseTestSpec( "*spaces" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == true );
        CHECK( spec.matches( *tcD ) == false );
        CHECK( parseTestSpec( "*a" ).matches( *tcA ) == true );
    }
    SECTION( "Wildcard at the end" ) {
        TestSpec spec = parseTestSpec( "long*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == true );
        CHECK( spec.matches( *tcD ) == false );
        CHECK( parseTestSpec( "a*" ).matches( *tcA ) == true );
    }
    SECTION( "Wildcard at both ends" ) {
        TestSpec spec = parseTestSpec( "*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == true );
        CHECK( spec.matches( *tcD ) == true );
        CHECK( parseTestSpec( "*a*" ).matches( *tcA ) == true );
    }
    SECTION( "Redundant wildcard at the start" ) {
        TestSpec spec = parseTestSpec( "*a" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == true );
        CHECK( spec.matches( *tcB ) == false );
    }
    SECTION( "Redundant wildcard at the end" ) {
        TestSpec spec = parseTestSpec( "a*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == true );
        CHECK( spec.matches( *tcB ) == false );
    }
    SECTION( "Redundant wildcard at both ends" ) {
        TestSpec spec = parseTestSpec( "*a*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == true );
        CHECK( spec.matches( *tcB ) == false );
    }
    SECTION( "Wildcard at both ends, redundant at start" ) {
        TestSpec spec = parseTestSpec( "*longer*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == true );
        CHECK( spec.matches( *tcD ) == true );
    }
    SECTION( "Just wildcard" ) {
        TestSpec spec = parseTestSpec( "*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == true );
        CHECK( spec.matches( *tcB ) == true );
        CHECK( spec.matches( *tcC ) == true );
        CHECK( spec.matches( *tcD ) == true );
    }

    SECTION( "Single tag" ) {
        TestSpec spec = parseTestSpec( "[one]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == true );
        CHECK( spec.matches( *tcC ) == false );
    }
    SECTION( "Single tag, two matches" ) {
        TestSpec spec = parseTestSpec( "[x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == true );
        CHECK( spec.matches( *tcC ) == true );
    }
    SECTION( "Two tags" ) {
        TestSpec spec = parseTestSpec( "[two][x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == true );
    }
    SECTION( "Two tags, spare separated" ) {
        TestSpec spec = parseTestSpec( "[two] [x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == true );
    }
    SECTION( "Wildcarded name and tag" ) {
        TestSpec spec = parseTestSpec( "*name*[x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == true );
        CHECK( spec.matches( *tcD ) == false );
    }
    SECTION( "Single tag exclusion" ) {
        TestSpec spec = parseTestSpec( "~[one]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == true );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == false );
    }
    SECTION( "One tag exclusion and one tag inclusion" ) {
        TestSpec spec = parseTestSpec( "~[two][x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == true );
        CHECK( spec.matches( *tcC ) == false );
    }
    SECTION( "One tag exclusion and one wldcarded name inclusion" ) {
        TestSpec spec = parseTestSpec( "~[two]*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == false );
        CHECK( spec.matches( *tcD ) == true );
    }
    SECTION( "One tag exclusion, using exclude:, and one wldcarded name inclusion" ) {
        TestSpec spec = parseTestSpec( "exclude:[two]*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == false );
        CHECK( spec.matches( *tcD ) == true );
    }
    SECTION( "name exclusion" ) {
        TestSpec spec = parseTestSpec( "~b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == true );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == false );
        CHECK( spec.matches( *tcD ) == true );
    }
    SECTION( "wildcarded name exclusion" ) {
        TestSpec spec = parseTestSpec( "~*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == true );
        CHECK( spec.matches( *tcB ) == true );
        CHECK( spec.matches( *tcC ) == false );
        CHECK( spec.matches( *tcD ) == false );
    }
    SECTION( "wildcarded name exclusion with tag inclusion" ) {
        TestSpec spec = parseTestSpec( "~*name*,[three]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == true );
        CHECK( spec.matches( *tcB ) == true );
        CHECK( spec.matches( *tcC ) == true );
        CHECK( spec.matches( *tcD ) == false );
    }
    SECTION( "wildcarded name exclusion, using exclude:, with tag inclusion" ) {
        TestSpec spec = parseTestSpec( "exclude:*name*,[three]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == true );
        CHECK( spec.matches( *tcB ) == true );
        CHECK( spec.matches( *tcC ) == true );
        CHECK( spec.matches( *tcD ) == false );
    }
    SECTION( "two wildcarded names" ) {
        TestSpec spec = parseTestSpec( "\"longer*\"\"*spaces\"" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == true );
        CHECK( spec.matches( *tcD ) == false );
    }
    SECTION( "empty tag" ) {
        TestSpec spec = parseTestSpec( "[]" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == false );
        CHECK( spec.matches( *tcD ) == false );
    }
    SECTION( "empty quoted name" ) {
        TestSpec spec = parseTestSpec( "\"\"" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == false );
        CHECK( spec.matches( *tcD ) == false );
    }
    SECTION( "quoted string followed by tag exclusion" ) {
        TestSpec spec = parseTestSpec( "\"*name*\"~[.]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( *tcA ) == false );
        CHECK( spec.matches( *tcB ) == false );
        CHECK( spec.matches( *tcC ) == false );
        CHECK( spec.matches( *tcD ) == true );
    }
    SECTION( "Leading and trailing spaces in test spec" ) {
        TestSpec spec = parseTestSpec( "\"  aardvark \"" );
        CHECK( spec.matches( *fakeTestCase( "  aardvark " ) ) );
        CHECK( spec.matches( *fakeTestCase( "  aardvark" ) ) );
        CHECK( spec.matches( *fakeTestCase( " aardvark " ) ) );
        CHECK( spec.matches( *fakeTestCase( "aardvark " ) ) );
        CHECK( spec.matches( *fakeTestCase( "aardvark" ) ) );

    }
    SECTION( "Leading and trailing spaces in test name" ) {
        TestSpec spec = parseTestSpec( "aardvark" );
        CHECK( spec.matches( *fakeTestCase( "  aardvark " ) ) );
        CHECK( spec.matches( *fakeTestCase( "  aardvark" ) ) );
        CHECK( spec.matches( *fakeTestCase( " aardvark " ) ) );
        CHECK( spec.matches( *fakeTestCase( "aardvark " ) ) );
        CHECK( spec.matches( *fakeTestCase( "aardvark" ) ) );
    }
    SECTION("Shortened hide tags are split apart when parsing") {
        TestSpec spec = parseTestSpec("[.foo]");
        CHECK(spec.matches(*fakeTestCase("hidden and foo", "[.][foo]")));
        CHECK_FALSE(spec.matches(*fakeTestCase("only foo", "[foo]")));
    }
    SECTION("Shortened hide tags also properly handle exclusion") {
        TestSpec spec = parseTestSpec("~[.foo]");
        CHECK_FALSE(spec.matches(*fakeTestCase("hidden and foo", "[.][foo]")));
        CHECK_FALSE(spec.matches(*fakeTestCase("only foo", "[foo]")));
        CHECK_FALSE(spec.matches(*fakeTestCase("only hidden", "[.]")));
        CHECK(spec.matches(*fakeTestCase("neither foo nor hidden", "[bar]")));
    }
}

TEST_CASE("#1905 -- test spec parser properly clears internal state between compound tests", "[command-line][test-spec]") {
    using Catch::parseTestSpec;
    using Catch::TestSpec;
    // We ask for one of 2 different tests and the latter one of them has a , in name that needs escaping
    TestSpec spec = parseTestSpec(R"("spec . char","spec \, char")");

    REQUIRE(spec.matches(*fakeTestCase("spec . char")));
    REQUIRE(spec.matches(*fakeTestCase("spec , char")));
    REQUIRE_FALSE(spec.matches(*fakeTestCase(R"(spec \, char)")));
}

TEST_CASE("#1912 -- test spec parser handles escaping", "[command-line][test-spec]") {
    using Catch::parseTestSpec;
    using Catch::TestSpec;

    SECTION("Various parentheses") {
        TestSpec spec = parseTestSpec(R"(spec {a} char,spec \[a] char)");

        REQUIRE(spec.matches(*fakeTestCase(R"(spec {a} char)")));
        REQUIRE(spec.matches(*fakeTestCase(R"(spec [a] char)")));
        REQUIRE_FALSE(spec.matches(*fakeTestCase("differs but has similar tag", "[a]")));
    }
    SECTION("backslash in test name") {
        TestSpec spec = parseTestSpec(R"(spec \\ char)");

        REQUIRE(spec.matches(*fakeTestCase(R"(spec \ char)")));
    }
}

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
        CHECK( config.reporterSpecifications.empty() );

        Catch::Config cfg(config);
        CHECK_FALSE(cfg.hasTestFilters());

        // The Config is responsible for mixing in the default reporter
        auto expectedReporter =
#if defined( CATCH_CONFIG_DEFAULT_REPORTER )
            CATCH_CONFIG_DEFAULT_REPORTER
#else
            "console"
#endif
            ;

        CHECK( cfg.getReporterSpecs().size() == 1 );
        CHECK( cfg.getReporterSpecs()[0] ==
               Catch::ReporterSpec{ expectedReporter, {}, {}, {} } );
        CHECK( cfg.getProcessedReporterSpecs().size() == 1 );
        CHECK( cfg.getProcessedReporterSpecs()[0] ==
               Catch::ProcessedReporterSpec{ expectedReporter,
                                             std::string{},
                                             Catch::ColourMode::PlatformDefault,
                                             {} } );
    }

    SECTION("test lists") {
        SECTION("Specify one test case using") {
            auto result = cli.parse({"test", "test1"});
            CHECK(result);

            Catch::Config cfg(config);
            REQUIRE(cfg.hasTestFilters());
            REQUIRE(cfg.testSpec().matches(*fakeTestCase("notIncluded")) == false);
            REQUIRE(cfg.testSpec().matches(*fakeTestCase("test1")));
        }
        SECTION("Specify one test case exclusion using exclude:") {
            auto result = cli.parse({"test", "exclude:test1"});
            CHECK(result);

            Catch::Config cfg(config);
            REQUIRE(cfg.hasTestFilters());
            REQUIRE(cfg.testSpec().matches(*fakeTestCase("test1")) == false);
            REQUIRE(cfg.testSpec().matches(*fakeTestCase("alwaysIncluded")));
        }

        SECTION("Specify one test case exclusion using ~") {
            auto result = cli.parse({"test", "~test1"});
            CHECK(result);

            Catch::Config cfg(config);
            REQUIRE(cfg.hasTestFilters());
            REQUIRE(cfg.testSpec().matches(*fakeTestCase("test1")) == false);
            REQUIRE(cfg.testSpec().matches(*fakeTestCase("alwaysIncluded")));
        }

    }

    SECTION("reporter") {
        using vec_Specs = std::vector<Catch::ReporterSpec>;
        using namespace std::string_literals;
        SECTION("-r/console") {
            auto result = cli.parse({"test", "-r", "console"});
            CAPTURE(result.errorMessage());
            CHECK(result);

            REQUIRE( config.reporterSpecifications ==
                     vec_Specs{ { "console", {}, {}, {} } } );
        }
        SECTION("-r/xml") {
            auto result = cli.parse({"test", "-r", "xml"});
            CAPTURE(result.errorMessage());
            CHECK(result);

            REQUIRE( config.reporterSpecifications ==
                     vec_Specs{ { "xml", {}, {}, {} } } );
        }
        SECTION("--reporter/junit") {
            auto result = cli.parse({"test", "--reporter", "junit"});
            CAPTURE(result.errorMessage());
            CHECK(result);

            REQUIRE( config.reporterSpecifications ==
                     vec_Specs{ { "junit", {}, {}, {} } } );
        }
        SECTION("must match one of the available ones") {
            auto result = cli.parse({"test", "--reporter", "unsupported"});
            CHECK(!result);

            REQUIRE_THAT(result.errorMessage(), ContainsSubstring("Unrecognized reporter"));
        }
        SECTION("With output file") {
            auto result = cli.parse({ "test", "-r", "console::out=out.txt" });
            CAPTURE(result.errorMessage());
            CHECK(result);
            REQUIRE( config.reporterSpecifications ==
                     vec_Specs{ { "console", "out.txt"s, {}, {} } } );
        }
        SECTION("With Windows-like absolute path as output file") {
            auto result = cli.parse({ "test", "-r", "console::out=C:\\Temp\\out.txt" });
            CAPTURE(result.errorMessage());
            CHECK(result);
            REQUIRE( config.reporterSpecifications ==
                     vec_Specs{ { "console", "C:\\Temp\\out.txt"s, {}, {} } } );
        }
        SECTION("Multiple reporters") {
            SECTION("All with output files") {
                CHECK(cli.parse({ "test", "-r", "xml::out=output.xml", "-r", "junit::out=output-junit.xml" }));
                REQUIRE( config.reporterSpecifications ==
                         vec_Specs{ { "xml", "output.xml"s, {}, {} },
                               { "junit", "output-junit.xml"s, {}, {} } } );
            }
            SECTION("Mixed output files and default output") {
                CHECK(cli.parse({ "test", "-r", "xml::out=output.xml", "-r", "console" }));
                REQUIRE( config.reporterSpecifications ==
                         vec_Specs{ { "xml", "output.xml"s, {}, {} },
                                    { "console", {}, {}, {} } } );
            }
            SECTION("cannot have multiple reporters with default output") {
                auto result = cli.parse({ "test", "-r", "console", "-r", "xml::out=output.xml", "-r", "junit" });
                CHECK(!result);
                REQUIRE_THAT(result.errorMessage(), ContainsSubstring("Only one reporter may have unspecified output file."));
            }
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
            REQUIRE_THAT(result.errorMessage(), ContainsSubstring("convert") && ContainsSubstring("oops"));
        }

     SECTION("wait-for-keypress") {
        SECTION("Accepted options") {
            using tuple_type = std::tuple<char const*, Catch::WaitForKeypress::When>;
            auto input = GENERATE(table<char const*, Catch::WaitForKeypress::When>({
                tuple_type{"never", Catch::WaitForKeypress::Never},
                tuple_type{"start", Catch::WaitForKeypress::BeforeStart},
                tuple_type{"exit",  Catch::WaitForKeypress::BeforeExit},
                tuple_type{"both",  Catch::WaitForKeypress::BeforeStartAndExit},
            }));
            CHECK(cli.parse({"test", "--wait-for-keypress", std::get<0>(input)}));

            REQUIRE(config.waitForKeypress == std::get<1>(input));
        }

        SECTION("invalid options are reported") {
            auto result = cli.parse({"test", "--wait-for-keypress", "sometimes"});
            CHECK(!result);

#ifndef CATCH_CONFIG_DISABLE_MATCHERS
            REQUIRE_THAT(result.errorMessage(), ContainsSubstring("never") && ContainsSubstring("both"));
#endif
        }
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

            REQUIRE(config.defaultOutputFilename == "filename.ext");
        }
        SECTION("--out") {
            CHECK(cli.parse({"test", "--out", "filename.ext"}));

            REQUIRE(config.defaultOutputFilename == "filename.ext");
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

        using Catch::ColourMode;

        SECTION( "without option" ) {
            CHECK(cli.parse({"test"}));

            REQUIRE( config.defaultColourMode == ColourMode::PlatformDefault );
        }

        SECTION( "auto" ) {
            CHECK( cli.parse( { "test", "--colour-mode", "default" } ) );

            REQUIRE( config.defaultColourMode == ColourMode::PlatformDefault );
        }

        SECTION( "yes" ) {
            CHECK(cli.parse({"test", "--colour-mode", "ansi"}));

            REQUIRE( config.defaultColourMode == ColourMode::ANSI );
        }

        SECTION( "no" ) {
            CHECK(cli.parse({"test", "--colour-mode", "none"}));

            REQUIRE( config.defaultColourMode == ColourMode::None );
        }

        SECTION( "error" ) {
            auto result = cli.parse({"test", "--colour-mode", "wrong"});
            CHECK( !result );
            CHECK_THAT( result.errorMessage(), ContainsSubstring( "colour mode must be one of" ) );
        }
    }

    SECTION("Benchmark options") {
        SECTION("samples") {
            CHECK(cli.parse({ "test", "--benchmark-samples=200" }));

            REQUIRE(config.benchmarkSamples == 200);
        }

        SECTION("resamples") {
            CHECK(cli.parse({ "test", "--benchmark-resamples=20000" }));

            REQUIRE(config.benchmarkResamples == 20000);
        }

        SECTION("confidence-interval") {
            CHECK(cli.parse({ "test", "--benchmark-confidence-interval=0.99" }));

            REQUIRE(config.benchmarkConfidenceInterval == Catch::Approx(0.99));
        }

        SECTION("no-analysis") {
            CHECK(cli.parse({ "test", "--benchmark-no-analysis" }));

            REQUIRE(config.benchmarkNoAnalysis);
        }

        SECTION("warmup-time") {
            CHECK(cli.parse({ "test", "--benchmark-warmup-time=10" }));

            REQUIRE(config.benchmarkWarmupTime == 10);
        }
    }
}

TEST_CASE("Parsing sharding-related cli flags", "[sharding]") {
    using namespace Catch::Matchers;

    Catch::ConfigData config;
    auto cli = Catch::makeCommandLineParser(config);

    SECTION("shard-count") {
        CHECK(cli.parse({ "test", "--shard-count=8" }));

        REQUIRE(config.shardCount == 8);
    }

    SECTION("Negative shard count reports error") {
        auto result = cli.parse({ "test", "--shard-count=-1" });

        CHECK_FALSE(result);
        REQUIRE_THAT(result.errorMessage(), ContainsSubstring("Shard count must be a positive number"));
    }

    SECTION("Zero shard count reports error") {
        auto result = cli.parse({ "test", "--shard-count=0" });

        CHECK_FALSE(result);
        REQUIRE_THAT(result.errorMessage(), ContainsSubstring("Shard count must be a positive number"));
    }

    SECTION("shard-index") {
        CHECK(cli.parse({ "test", "--shard-index=2" }));

        REQUIRE(config.shardIndex == 2);
    }

    SECTION("Negative shard index reports error") {
        auto result = cli.parse({ "test", "--shard-index=-12" });

        CHECK_FALSE(result);
        REQUIRE_THAT(result.errorMessage(), ContainsSubstring("Shard index must be a non-negative number"));
    }

    SECTION("Shard index 0 is accepted") {
        CHECK(cli.parse({ "test", "--shard-index=0" }));

        REQUIRE(config.shardIndex == 0);
    }

}

TEST_CASE( "Parsing warnings", "[cli][warnings]" ) {
    using Catch::WarnAbout;

    Catch::ConfigData config;
    auto cli = Catch::makeCommandLineParser( config );

    SECTION( "NoAssertions" ) {
        REQUIRE(cli.parse( { "test", "-w", "NoAssertions" } ));
        REQUIRE( config.warnings == WarnAbout::NoAssertions );
    }
    SECTION( "NoTests is no longer supported" ) {
        REQUIRE_FALSE(cli.parse( { "test", "-w", "NoTests" } ));
    }
    SECTION( "Combining multiple warnings" ) {
        REQUIRE( cli.parse( { "test",
                              "--warn", "NoAssertions",
                              "--warn", "UnmatchedTestSpec" } ) );

        REQUIRE( config.warnings == ( WarnAbout::NoAssertions | WarnAbout::UnmatchedTestSpec ) );
    }
}

TEST_CASE("Test with special, characters \"in name", "[cli][regression]") {
    // This test case succeeds if we can invoke it from the CLI
    SUCCEED();
}

TEST_CASE("Various suspicious reporter specs are rejected",
          "[cli][reporter-spec][approvals]") {
    Catch::ConfigData config;
    auto cli = Catch::makeCommandLineParser( config );

    auto spec = GENERATE( as<std::string>{},
                          "",
                          "::console",
                          "console::",
                          "console::some-file::",
                          "::console::some-file::" );
    CAPTURE( spec );

    auto result = cli.parse( { "test", "--reporter", spec } );
    REQUIRE_FALSE( result );
}

TEST_CASE("Win32 colour implementation is compile-time optional",
          "[approvals][cli][colours]") {
    Catch::ConfigData config;
    auto cli = Catch::makeCommandLineParser( config );

    auto result = cli.parse( { "test", "--colour-mode", "win32" } );

#if defined( CATCH_CONFIG_COLOUR_WIN32 )
    REQUIRE( result );
#else
    REQUIRE_FALSE( result );
#endif
}
