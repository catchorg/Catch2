/*
 *  Created by Phil on 27/11/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_BASES_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_BASES_HPP_INCLUDED

#include "../internal/catch_interfaces_reporter.h"

#include <cstring>
#include <cfloat>
#include <cstdio>
#include <assert.h>
#include <memory>

namespace Catch {

    // Returns double formatted as %.3f (format expected on output)
    std::string getFormattedDuration( double duration );

    struct StreamingReporterBase : IStreamingReporter {

        StreamingReporterBase(ReporterConfig const& _config);

        virtual ReporterPreferences getPreferences() const override;

        virtual ~StreamingReporterBase() override;

        virtual void noMatchingTestCases(std::string const&) override;

        virtual void testRunStarting(TestRunInfo const& _testRunInfo) override;
        virtual void testGroupStarting(GroupInfo const& _groupInfo) override;

        virtual void testCaseStarting(TestCaseInfo const& _testInfo) override;
        virtual void sectionStarting(SectionInfo const& _sectionInfo) override;

        virtual void sectionEnded(SectionStats const& /* _sectionStats */) override;
        virtual void testCaseEnded(TestCaseStats const& /* _testCaseStats */) override;
        virtual void testGroupEnded(TestGroupStats const& /* _testGroupStats */) override;
        virtual void testRunEnded(TestRunStats const& /* _testRunStats */) override;

        virtual void skipTest(TestCaseInfo const&) override;

        IConfigPtr m_config;
        std::ostream& stream;

        LazyStat<TestRunInfo> currentTestRunInfo;
        LazyStat<GroupInfo> currentGroupInfo;
        LazyStat<TestCaseInfo> currentTestCaseInfo;

        std::vector<SectionInfo> m_sectionStack;
        ReporterPreferences m_reporterPrefs;
    };

    struct CumulativeReporterBase : IStreamingReporter {
        template<typename T, typename ChildNodeT>
        struct Node {
            explicit Node( T const& _value ) : value( _value ) {}
            virtual ~Node() {}

            using ChildNodes = std::vector<std::shared_ptr<ChildNodeT>>;
            T value;
            ChildNodes children;
        };
        struct SectionNode {
            explicit SectionNode(SectionStats const& _stats);
            virtual ~SectionNode();

            bool operator == (SectionNode const& other) const;
            bool operator == (std::shared_ptr<SectionNode> const& other) const;

            SectionStats stats;
            using ChildSections = std::vector<std::shared_ptr<SectionNode>>;
            using Assertions = std::vector<AssertionStats>;
            ChildSections childSections;
            Assertions assertions;
            std::string stdOut;
            std::string stdErr;
        };

        struct BySectionInfo {
            BySectionInfo(SectionInfo const& other);
            BySectionInfo(BySectionInfo const& other);
            bool operator() (std::shared_ptr<SectionNode> const& node) const;
            void operator=(BySectionInfo const&) = delete;

        private:
            SectionInfo const& m_other;
        };


        using TestCaseNode = Node<TestCaseStats, SectionNode>;
        using TestGroupNode = Node<TestGroupStats, TestCaseNode>;
        using TestRunNode = Node<TestRunStats, TestGroupNode>;

        CumulativeReporterBase(ReporterConfig const& _config);
        ~CumulativeReporterBase();

        virtual ReporterPreferences getPreferences() const override;

        virtual void testRunStarting(TestRunInfo const&) override;
        virtual void testGroupStarting(GroupInfo const&) override;

        virtual void testCaseStarting(TestCaseInfo const&) override;

        virtual void sectionStarting(SectionInfo const& sectionInfo) override;

        virtual void assertionStarting(AssertionInfo const&) override;

        virtual bool assertionEnded(AssertionStats const& assertionStats) override;
        virtual void sectionEnded(SectionStats const& sectionStats) override;
        virtual void testCaseEnded(TestCaseStats const& testCaseStats) override;
        virtual void testGroupEnded(TestGroupStats const& testGroupStats) override;
        virtual void testRunEnded(TestRunStats const& testRunStats) override;
        virtual void testRunEndedCumulative() = 0;

        virtual void skipTest(TestCaseInfo const&) override;

        virtual void prepareExpandedExpression(AssertionResult& result) const;

        IConfigPtr m_config;
        std::ostream& stream;
        std::vector<AssertionStats> m_assertions;
        std::vector<std::vector<std::shared_ptr<SectionNode>>> m_sections;
        std::vector<std::shared_ptr<TestCaseNode>> m_testCases;
        std::vector<std::shared_ptr<TestGroupNode>> m_testGroups;

        std::vector<std::shared_ptr<TestRunNode>> m_testRuns;

        std::shared_ptr<SectionNode> m_rootSection;
        std::shared_ptr<SectionNode> m_deepestSection;
        std::vector<std::shared_ptr<SectionNode>> m_sectionStack;
        ReporterPreferences m_reporterPrefs;
    };

    template<char C>
    char const* getLineOfChars() {
        static char line[CATCH_CONFIG_CONSOLE_WIDTH] = {0};
        if( !*line ) {
            std::memset( line, C, CATCH_CONFIG_CONSOLE_WIDTH-1 );
            line[CATCH_CONFIG_CONSOLE_WIDTH-1] = 0;
        }
        return line;
    }


    struct TestEventListenerBase : StreamingReporterBase {
        TestEventListenerBase(ReporterConfig const& _config);

        virtual void assertionStarting(AssertionInfo const&) override;
        virtual bool assertionEnded(AssertionStats const&) override;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_BASES_HPP_INCLUDED
