/*
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/reporters/catch_reporter_automake.hpp>

namespace Catch {

    AutomakeReporter::~AutomakeReporter() {}

    void AutomakeReporter::testCaseEnded(TestCaseStats const& _testCaseStats) {
        // Possible values to emit are PASS, XFAIL, SKIP, FAIL, XPASS and ERROR.
        stream << ":test-result: ";
        if (_testCaseStats.totals.assertions.allPassed()) {
            stream << "PASS";
        } else if (_testCaseStats.totals.assertions.allOk()) {
            stream << "XFAIL";
        } else {
            stream << "FAIL";
        }
        stream << ' ' << _testCaseStats.testInfo->name << '\n';
        StreamingReporterBase::testCaseEnded(_testCaseStats);
    }

    void AutomakeReporter::skipTest(TestCaseInfo const& testInfo) {
        stream << ":test-result: SKIP " << testInfo.name << '\n';
    }

} // end namespace Catch
