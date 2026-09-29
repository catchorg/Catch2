
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

/**\file
 * Runs simple test cases with reporter that likes to throw.
 *
 * Used to analyze how Catch2 currently treats throwing reporters/listeners,
 * and what changes make sense for the future.
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <catch2/reporters/catch_reporter_streaming_base.hpp>

namespace {

    class ThrowingReporter final : public Catch::StreamingReporterBase {
    public:
        ThrowingReporter( Catch::ReporterConfig&& config ):
            StreamingReporterBase( CATCH_MOVE( config ) ) {
            m_preferences.shouldReportAllAssertions = true;
            m_preferences.shouldReportAllAssertionStarts = false;
            throw std::runtime_error( "Exception in constructor" );
        }

        static std::string getDescription() {
            using namespace std::string_literals;
            return "Throws for different reporter events"s;
        }
    };

    CATCH_REGISTER_REPORTER( "ThrowingReporter", ThrowingReporter )

} // namespace

TEST_CASE( "Basic test case" ) {}
