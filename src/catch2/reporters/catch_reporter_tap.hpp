/*
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_TAP_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_TAP_HPP_INCLUDED

#include <catch2/reporters/catch_reporter_bases.hpp>

namespace Catch {

    struct TAPReporter : StreamingReporterBase {

        using StreamingReporterBase::StreamingReporterBase;

        ~TAPReporter() override;

        static std::string getDescription() {
            using namespace std::string_literals;
            return "Reports test results in TAP format, suitable for test harnesses"s;
        }

        ReporterPreferences getPreferences() const override {
            return m_reporterPrefs;
        }

        void noMatchingTestCases(std::string const& spec) override;

        void assertionStarting( AssertionInfo const& ) override {}

        bool assertionEnded(AssertionStats const& _assertionStats) override;

        void testRunEnded(TestRunStats const& _testRunStats) override;

    private:
        std::size_t counter = 0;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_TAP_HPP_INCLUDED
