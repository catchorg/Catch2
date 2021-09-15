
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_CUMULATIVE_BASE_HPP_INCLUDED
#define CATCH_REPORTER_CUMULATIVE_BASE_HPP_INCLUDED

#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>

#include <iosfwd>
#include <string>
#include <vector>

namespace Catch {

    struct CumulativeReporterBase : IStreamingReporter {
        template<typename T, typename ChildNodeT>
        struct Node {
            explicit Node( T const& _value ) : value( _value ) {}

            using ChildNodes = std::vector<Detail::unique_ptr<ChildNodeT>>;
            T value;
            ChildNodes children;
        };
        struct SectionNode {
            explicit SectionNode(SectionStats const& _stats) : stats(_stats) {}

            bool operator == (SectionNode const& other) const {
                return stats.sectionInfo.lineInfo == other.stats.sectionInfo.lineInfo;
            }

            SectionStats stats;
            std::vector<Detail::unique_ptr<SectionNode>> childSections;
            std::vector<AssertionStats> assertions;
            std::string stdOut;
            std::string stdErr;
        };


        using TestCaseNode = Node<TestCaseStats, SectionNode>;
        using TestRunNode = Node<TestRunStats, TestCaseNode>;

        CumulativeReporterBase( ReporterConfig const& _config ):
            IStreamingReporter( _config.fullConfig() ),
            stream( _config.stream() ) {}
        ~CumulativeReporterBase() override;

        void benchmarkPreparing( StringRef ) override {}
        void benchmarkStarting( BenchmarkInfo const& ) override {}
        void benchmarkEnded( BenchmarkStats<> const& ) override {}
        void benchmarkFailed( StringRef ) override {}

        void noMatchingTestCases( StringRef ) override {}
        void reportInvalidArguments( StringRef ) override {}
        void fatalErrorEncountered( StringRef /*error*/ ) override {}


        void testRunStarting( TestRunInfo const& ) override {}

        void testCaseStarting( TestCaseInfo const& ) override {}
        void testCasePartialStarting( TestCaseInfo const&, uint64_t ) override {}
        void sectionStarting( SectionInfo const& sectionInfo ) override;

        void assertionStarting( AssertionInfo const& ) override {}

        void assertionEnded( AssertionStats const& assertionStats ) override;
        void sectionEnded( SectionStats const& sectionStats ) override;
        void testCasePartialEnded( TestCaseStats const&, uint64_t ) override {}
        void testCaseEnded( TestCaseStats const& testCaseStats ) override;
        void testRunEnded( TestRunStats const& testRunStats ) override;
        //! Customization point: called after last test finishes (testRunEnded has been handled)
        virtual void testRunEndedCumulative() = 0;

        void skipTest(TestCaseInfo const&) override {}

        void listReporters( std::vector<ReporterDescription> const& descriptions ) override;
        void listTests( std::vector<TestCaseHandle> const& tests ) override;
        void listTags( std::vector<TagInfo> const& tags ) override;

    protected:
        std::ostream& stream;
        // Note: We rely on pointer identity being stable, which is why
        //       we store pointers to the nodes rather than the values.
        std::vector<Detail::unique_ptr<TestCaseNode>> m_testCases;
        // We need lazy construction here. We should probably refactor it
        // later, after the events are redone.
        Detail::unique_ptr<TestRunNode> m_testRun;

        Detail::unique_ptr<SectionNode> m_rootSection;
        SectionNode* m_deepestSection = nullptr;
        std::vector<SectionNode*> m_sectionStack;
    };

} // end namespace Catch

#endif // CATCH_REPORTER_CUMULATIVE_BASE_HPP_INCLUDED
