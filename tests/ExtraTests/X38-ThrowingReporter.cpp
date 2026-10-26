
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
#include <catch2/reporters/catch_reporter_helpers.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <catch2/reporters/catch_reporter_streaming_base.hpp>

namespace {

    class ThrowingReporter final : public Catch::StreamingReporterBase {
        bool m_throwInConstructor = false;
        bool m_throwInAssertionEnded = false;

        void processOptions() {
            if ( m_customOptions.find( "Xthrow-in-constructor" ) !=
                 m_customOptions.end() ) {
                m_throwInConstructor = true;
            }
            if ( m_customOptions.find( "Xthrow-in-assertion-ended" ) !=
                 m_customOptions.end() ) {
                m_throwInAssertionEnded = true;
            }
        }

    public:
        ThrowingReporter( Catch::ReporterConfig&& config ):
            StreamingReporterBase( CATCH_MOVE( config ) ) {
            m_preferences.shouldReportAllAssertions = true;
            m_preferences.shouldReportAllAssertionStarts = false;

            Catch::rejectSuperfluousConfigKeys(
                m_customOptions,
                { "Xthrow-in-constructor", "Xthrow-in-assertion-ended" } );
            processOptions();

            if ( m_throwInConstructor ) {
                throw std::runtime_error( "Exception in constructor" );
            }
        }

        void assertionEnded( Catch::AssertionStats const& stats ) override {
            m_stream << "assertionEnded\n";
            if ( m_throwInAssertionEnded ) {
                throw std::runtime_error( "Exception in assertionEnded" );
            }
        }

        static std::string getDescription() {
            using namespace std::string_literals;
            return "Throws for different reporter events"s;
        }
    };

    CATCH_REGISTER_REPORTER( "ThrowingReporter", ThrowingReporter )

} // namespace

TEST_CASE( "REQUIRE - Pass" ) { REQUIRE( true ); }

TEST_CASE( "REQUIRE - Fail" ) { REQUIRE( false ); }

TEST_CASE( "REQUIRE_THROWS - Pass" ) {
    REQUIRE_THROWS( [] {
        throw std::runtime_error( "expected" );
    }() );
}
