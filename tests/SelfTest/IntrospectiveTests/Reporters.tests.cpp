
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>

#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_config.hpp>
#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <catch2/interfaces/catch_interfaces_reporter_factory.hpp>
#include <catch2/interfaces/catch_interfaces_reporter_registry.hpp>
#include <catch2/internal/catch_list.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/reporters/catch_reporter_helpers.hpp>

#include <sstream>

TEST_CASE( "The default listing implementation write to provided stream",
           "[reporters][reporter-helpers]" ) {
    using Catch::Matchers::Contains;
    using namespace std::string_literals;

    std::stringstream sstream;
    SECTION( "Listing tags" ) {
        std::vector<Catch::TagInfo> tags(1);
        tags[0].add("fakeTag"_catch_sr);
        Catch::defaultListTags(sstream, tags, false);

        auto listingString = sstream.str();
        REQUIRE_THAT(listingString, Contains("[fakeTag]"s));
    }
    SECTION( "Listing reporters" ) {
        std::vector<Catch::ReporterDescription> reporters(
            { { "fake reporter", "fake description" } } );
        Catch::defaultListReporters(sstream, reporters, Catch::Verbosity::Normal);

        auto listingString = sstream.str();
        REQUIRE_THAT(listingString, Contains("fake reporter"s));
    }
    SECTION( "Listing tests" ) {
        Catch::TestCaseInfo fakeInfo{
            ""s,
            { "fake test name"_catch_sr, "[fakeTestTag]"_catch_sr },
            { "fake-file.cpp", 123456789 } };
        std::vector<Catch::TestCaseHandle> tests({ {&fakeInfo, nullptr} });
        Catch::defaultListTests(sstream, tests, false, Catch::Verbosity::Normal);

        auto listingString = sstream.str();
        REQUIRE_THAT( listingString,
                      Contains( "fake test name"s ) &&
                          Contains( "fakeTestTag"s ) );
    }
}

TEST_CASE( "Reporter's write listings to provided stream", "[reporters]" ) {
    using Catch::Matchers::Contains;
    using namespace std::string_literals;

    auto const& factories = Catch::getRegistryHub().getReporterRegistry().getFactories();
    // If there are no reporters, the test would pass falsely
    // while there is something obviously broken
    REQUIRE_FALSE(factories.empty());

    for (auto const& factory : factories) {
        INFO("Tested reporter: " << factory.first);
        std::stringstream sstream;

        Catch::ConfigData config_data;
        Catch::Config config( config_data );
        Catch::ReporterConfig rep_config( &config, sstream );
        auto reporter = factory.second->create( rep_config );

        DYNAMIC_SECTION( factory.first << " reporter lists tags" ) {
            std::vector<Catch::TagInfo> tags(1);
            tags[0].add("fakeTag"_catch_sr);
            reporter->listTags(tags);

            auto listingString = sstream.str();
            REQUIRE_THAT(listingString, Contains("fakeTag"s));
        }

        DYNAMIC_SECTION( factory.first << " reporter lists reporters" ) {
            std::vector<Catch::ReporterDescription> reporters(
                { { "fake reporter", "fake description" } } );
            reporter->listReporters(reporters);

            auto listingString = sstream.str();
            REQUIRE_THAT(listingString, Contains("fake reporter"s));
        }

        DYNAMIC_SECTION( factory.first << " reporter lists tests" ) {
            Catch::TestCaseInfo fakeInfo{
                ""s,
                { "fake test name"_catch_sr, "[fakeTestTag]"_catch_sr },
                { "fake-file.cpp", 123456789 } };
            std::vector<Catch::TestCaseHandle> tests({ {&fakeInfo, nullptr} });
            reporter->listTests(tests);

            auto listingString = sstream.str();
            REQUIRE_THAT( listingString,
                          Contains( "fake test name"s ) &&
                              Contains( "fakeTestTag"s ) );
        }
    }
}
