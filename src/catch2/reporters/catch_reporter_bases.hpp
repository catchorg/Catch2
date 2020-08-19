/*
 *  Created by Phil on 27/11/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_BASES_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_BASES_HPP_INCLUDED

#include <catch2/interfaces/catch_interfaces_reporter.hpp>

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace Catch {
    // Returns double formatted as %.3f (format expected on output)
    std::string getFormattedDuration( double duration );

    //! Should the reporter show
    bool shouldShowDuration( IConfig const& config, double duration );

    std::string serializeFilters( std::vector<std::string> const& container );

    struct StreamingReporterBase : IStreamingReporter {

        StreamingReporterBase( ReporterConfig const& _config ):
            m_config( _config.fullConfig() ), stream( _config.stream() ) {
        }


        ~StreamingReporterBase() override;

        void noMatchingTestCases(std::string const&) override {}

        void reportInvalidArguments(std::string const&) override {}

        void testRunStarting(TestRunInfo const& _testRunInfo) override {
            currentTestRunInfo = _testRunInfo;
        }

        void testGroupStarting(GroupInfo const& _groupInfo) override {
            currentGroupInfo = _groupInfo;
        }

        void testCaseStarting(TestCaseInfo const& _testInfo) override  {
            currentTestCaseInfo = &_testInfo;
        }
        void sectionStarting(SectionInfo const& _sectionInfo) override {
            m_sectionStack.push_back(_sectionInfo);
        }

        void sectionEnded(SectionStats const& /* _sectionStats */) override {
            m_sectionStack.pop_back();
        }
        void testCaseEnded(TestCaseStats const& /* _testCaseStats */) override {
            currentTestCaseInfo = nullptr;
        }
        void testGroupEnded( TestGroupStats const& ) override;
        void testRunEnded( TestRunStats const& /* _testRunStats */ ) override;

        void skipTest(TestCaseInfo const&) override {
            // Don't do anything with this by default.
            // It can optionally be overridden in the derived class.
        }

        IConfig const* m_config;
        std::ostream& stream;

        LazyStat<TestRunInfo> currentTestRunInfo;
        LazyStat<GroupInfo> currentGroupInfo;
        TestCaseInfo const* currentTestCaseInfo = nullptr;

        std::vector<SectionInfo> m_sectionStack;
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
            explicit SectionNode(SectionStats const& _stats) : stats(_stats) {}

            bool operator == (SectionNode const& other) const {
                return stats.sectionInfo.lineInfo == other.stats.sectionInfo.lineInfo;
            }

            SectionStats stats;
            using ChildSections = std::vector<std::shared_ptr<SectionNode>>;
            using Assertions = std::vector<AssertionStats>;
            ChildSections childSections;
            Assertions assertions;
            std::string stdOut;
            std::string stdErr;
        };


        using TestCaseNode = Node<TestCaseStats, SectionNode>;
        using TestGroupNode = Node<TestGroupStats, TestCaseNode>;
        using TestRunNode = Node<TestRunStats, TestGroupNode>;

        CumulativeReporterBase( ReporterConfig const& _config ):
            m_config( _config.fullConfig() ), stream( _config.stream() ) {}
        ~CumulativeReporterBase() override;

        void testRunStarting( TestRunInfo const& ) override {}
        void testGroupStarting( GroupInfo const& ) override {}

        void testCaseStarting( TestCaseInfo const& ) override {}

        void sectionStarting( SectionInfo const& sectionInfo ) override;

        void assertionStarting( AssertionInfo const& ) override {}

        bool assertionEnded( AssertionStats const& assertionStats ) override;
        void sectionEnded( SectionStats const& sectionStats ) override;
        void testCaseEnded( TestCaseStats const& testCaseStats ) override;
        void testGroupEnded( TestGroupStats const& testGroupStats ) override;
        void testRunEnded( TestRunStats const& testRunStats ) override;
        virtual void testRunEndedCumulative() = 0;

        void skipTest(TestCaseInfo const&) override {}

        IConfig const* m_config;
        std::ostream& stream;
        std::vector<AssertionStats> m_assertions;
        std::vector<std::vector<std::shared_ptr<SectionNode>>> m_sections;
        std::vector<std::shared_ptr<TestCaseNode>> m_testCases;
        std::vector<std::shared_ptr<TestGroupNode>> m_testGroups;

        std::vector<std::shared_ptr<TestRunNode>> m_testRuns;

        std::shared_ptr<SectionNode> m_rootSection;
        std::shared_ptr<SectionNode> m_deepestSection;
        std::vector<std::shared_ptr<SectionNode>> m_sectionStack;
    };

    struct lineOfChars {
        char c;
        constexpr lineOfChars(char c):
            c(c)
        {}

        friend std::ostream& operator<< (std::ostream& out, lineOfChars value);
    };

    struct TestEventListenerBase : StreamingReporterBase {
        TestEventListenerBase( ReporterConfig const& _config );

        void assertionStarting(AssertionInfo const&) override;
        bool assertionEnded(AssertionStats const&) override;

        // Event listeners should not use the default listing impl
        void listReporters(std::vector<ReporterDescription> const&, IConfig const&) override {}
        void listTests(std::vector<TestCaseHandle> const&, IConfig const&) override {}
        void listTags(std::vector<TagInfo> const&, IConfig const&) override {}
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_BASES_HPP_INCLUDED