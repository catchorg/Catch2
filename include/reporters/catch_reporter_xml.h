/*
 *  Created by Martin on 14/11/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_XML_H_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_XML_H_INCLUDED

#include "catch_reporter_bases.hpp"

#include "../internal/catch_xmlwriter.h"
#include "../internal/catch_timer.h"


namespace Catch {
    class XmlReporter : public StreamingReporterBase<XmlReporter> {
    public:
        XmlReporter(ReporterConfig const& _config);

        ~XmlReporter() override;

        static std::string getDescription();

        virtual std::string getStylesheetRef() const;

        void writeSourceInfo(SourceLineInfo const& sourceInfo);

    public: // StreamingReporterBase

        void noMatchingTestCases(std::string const& s) override;

        void testRunStarting(TestRunInfo const& testInfo) override;

        void testGroupStarting(GroupInfo const& groupInfo) override;

        void testCaseStarting(TestCaseInfo const& testInfo) override;

        void sectionStarting(SectionInfo const& sectionInfo) override;

        void assertionStarting(AssertionInfo const&) override;

        bool assertionEnded(AssertionStats const& assertionStats) override;

        void sectionEnded(SectionStats const& sectionStats) override;

        void testCaseEnded(TestCaseStats const& testCaseStats) override;

        void testGroupEnded(TestGroupStats const& testGroupStats) override;

        void testRunEnded(TestRunStats const& testRunStats) override;

    private:
        Timer m_testCaseTimer;
        XmlWriter m_xml;
        int m_sectionDepth = 0;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_XML_H_INCLUDED
