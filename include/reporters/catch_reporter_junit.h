/*
 *  Created by Martin on 14/11/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_JUNIT_H_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_JUNIT_H_INCLUDED


#include "catch_reporter_bases.hpp"
#include "../internal/catch_xmlwriter.h"
#include "../internal/catch_timer.h"

namespace Catch {

    class JunitReporter : public CumulativeReporterBase<JunitReporter> {
    public:
        JunitReporter(ReporterConfig const& _config);

        ~JunitReporter() override;

        static std::string getDescription();

        void noMatchingTestCases(std::string const& /*spec*/) override;

        void testRunStarting(TestRunInfo const& runInfo) override;

        void testGroupStarting(GroupInfo const& groupInfo) override;

        void testCaseStarting(TestCaseInfo const& testCaseInfo) override;
        bool assertionEnded(AssertionStats const& assertionStats) override;

        void testCaseEnded(TestCaseStats const& testCaseStats) override;

        void testGroupEnded(TestGroupStats const& testGroupStats) override;

        void testRunEndedCumulative() override;

        void writeGroup(TestGroupNode const& groupNode, double suiteTime);

        void writeTestCase(TestCaseNode const& testCaseNode);

        void writeSection(std::string const& className,
                          std::string const& rootName,
                          SectionNode const& sectionNode);

        void writeAssertions(SectionNode const& sectionNode);
        void writeAssertion(AssertionStats const& stats);

        XmlWriter xml;
        Timer suiteTimer;
        std::string stdOutForSuite;
        std::string stdErrForSuite;
        unsigned int unexpectedExceptions = 0;
        bool m_okToFail = false;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_JUNIT_H_INCLUDED
