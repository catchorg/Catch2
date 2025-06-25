
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_TEAMCITY_HPP_INCLUDED
#define CATCH_REPORTER_TEAMCITY_HPP_INCLUDED

#include <catch2/reporters/catch_reporter_streaming_base.hpp>
#include <catch2/catch_timer.hpp>

#include <cstring>
#include <string>

#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {

    class TeamCityReporter final : public StreamingReporterBase {
    public:
        TeamCityReporter( ReporterConfig&& _config );

        ~TeamCityReporter() override;

        static std::string getDescription() {
            using namespace std::string_literals;
            return "Reports test results as TeamCity service messages"s;
        }

        void testRunStarting( TestRunInfo const& runInfo ) override;
        void testRunEnded( TestRunStats const& runStats ) override;

        void assertionEnded(AssertionStats const& assertionStats) override;

        void sectionStarting( SectionInfo const& sectionInfo ) override;
        void sectionEnded( SectionStats const& sectionStats ) override;

        void testCaseStarting(TestCaseInfo const& testInfo) override;

        void testCaseEnded(TestCaseStats const& testCaseStats) override;

    private:
        std::string printSectionName();
        std::string createTestCaseHeader( std::string name );
        std::string createTestCaseFooter( std::string name, double duration );
        void parseCustomOptions();

        Timer m_testCaseTimer;
        bool m_headerPrintedForThisSection{ false };
        bool m_printSections{ false };
        std::string m_sectionSeparator{ "." };
    };

} // end namespace Catch

#ifdef __clang__
#   pragma clang diagnostic pop
#endif

#endif // CATCH_REPORTER_TEAMCITY_HPP_INCLUDED
