/*
 *  Created by Phil on 27/11/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_BASES_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_BASES_HPP_INCLUDED

#include "../internal/catch_enforce.h"
#include "../internal/catch_interfaces_reporter.h"

#include <algorithm>
#include <cstring>
#include <cfloat>
#include <cstdio>
#include <cassert>
#include <memory>
#include <ostream>

namespace Catch {
    void prepareExpandedExpression(AssertionResult& result);

    // Returns double formatted as %.3f (format expected on output)
    std::string getFormattedDuration( double duration );

    template<typename DerivedT>
    struct StreamingReporterBase : IStreamingReporter {

        StreamingReporterBase( ReporterConfig const& _config )
        :   m_config( _config.fullConfig() ),
            stream( _config.stream() )
        {
            m_reporterPrefs.shouldRedirectStdOut = false;
            if( !DerivedT::getSupportedVerbosities().count( m_config->verbosity() ) )
                CATCH_ERROR( "Verbosity level not supported by this reporter" );
        }

        ReporterPreferences getPreferences() const override {
            return m_reporterPrefs;
        }

        static std::set<Verbosity> getSupportedVerbosities() {
            return { Verbosity::Normal };
        }

        ~StreamingReporterBase() override = default;

        void noMatchingTestCases(std::string const&) override {}

        void testRunStarting(TestRunInfo const& _testRunInfo) override {
            currentTestRunInfo = _testRunInfo;
        }
        void testGroupStarting(GroupInfo const& _groupInfo) override {
            currentGroupInfo = _groupInfo;
        }

        void testCaseStarting(TestCaseInfo const& _testInfo) override  {
            currentTestCaseInfo = _testInfo;
        }
        void sectionStarting(SectionInfo const& _sectionInfo) override {
            m_sectionStack.push_back(_sectionInfo);
        }

        void sectionEnded(SectionStats const& /* _sectionStats */) override {
            m_sectionStack.pop_back();
        }
        void testCaseEnded(TestCaseStats const& /* _testCaseStats */) override {
            currentTestCaseInfo.reset();
        }
        void testGroupEnded(TestGroupStats const& /* _testGroupStats */) override {
            currentGroupInfo.reset();
        }
        void testRunEnded(TestRunStats const& /* _testRunStats */) override {
            currentTestCaseInfo.reset();
            currentGroupInfo.reset();
            currentTestRunInfo.reset();
        }

        void skipTest(TestCaseInfo const&) override {
            // Don't do anything with this by default.
            // It can optionally be overridden in the derived class.
        }

        IConfigPtr m_config;
        std::ostream& stream;

        LazyStat<TestRunInfo> currentTestRunInfo;
        LazyStat<GroupInfo> currentGroupInfo;
        LazyStat<TestCaseInfo> currentTestCaseInfo;

        std::vector<SectionInfo> m_sectionStack;
        ReporterPreferences m_reporterPrefs;
    };

    template<typename DerivedT>
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
            virtual ~SectionNode() = default;

            bool operator == (SectionNode const& other) const {
                return stats.sectionInfo.lineInfo == other.stats.sectionInfo.lineInfo;
            }
            bool operator == (std::shared_ptr<SectionNode> const& other) const {
                return operator==(*other);
            }

            SectionStats stats;
            using ChildSections = std::vector<std::shared_ptr<SectionNode>>;
            using Assertions = std::vector<AssertionStats>;
            ChildSections childSections;
            Assertions assertions;
            std::string stdOut;
            std::string stdErr;
        };

        struct BySectionInfo {
            BySectionInfo( SectionInfo const& other ) : m_other( other ) {}
            BySectionInfo( BySectionInfo const& other ) : m_other( other.m_other ) {}
            bool operator() (std::shared_ptr<SectionNode> const& node) const {
                return ((node->stats.sectionInfo.name == m_other.name) &&
                        (node->stats.sectionInfo.lineInfo == m_other.lineInfo));
            }
            void operator=(BySectionInfo const&) = delete;

        private:
            SectionInfo const& m_other;
        };


        using TestCaseNode = Node<TestCaseStats, SectionNode>;
        using TestGroupNode = Node<TestGroupStats, TestCaseNode>;
        using TestRunNode = Node<TestRunStats, TestGroupNode>;

        CumulativeReporterBase( ReporterConfig const& _config )
        :   m_config( _config.fullConfig() ),
            stream( _config.stream() )
        {
            m_reporterPrefs.shouldRedirectStdOut = false;
            if( !DerivedT::getSupportedVerbosities().count( m_config->verbosity() ) )
                CATCH_ERROR( "Verbosity level not supported by this reporter" );
        }
        ~CumulativeReporterBase() override = default;

        ReporterPreferences getPreferences() const override {
            return m_reporterPrefs;
        }

        static std::set<Verbosity> getSupportedVerbosities() {
            return { Verbosity::Normal };
        }

        void testRunStarting( TestRunInfo const& ) override {}
        void testGroupStarting( GroupInfo const& ) override {}

        void testCaseStarting( TestCaseInfo const& ) override {}

        void sectionStarting( SectionInfo const& sectionInfo ) override {
            SectionStats incompleteStats( sectionInfo, Counts(), 0, false );
            std::shared_ptr<SectionNode> node;
            if( m_sectionStack.empty() ) {
                if( !m_rootSection )
                    m_rootSection = std::make_shared<SectionNode>( incompleteStats );
                node = m_rootSection;
            }
            else {
                SectionNode& parentNode = *m_sectionStack.back();
                auto it =
                    std::find_if(   parentNode.childSections.begin(),
                                    parentNode.childSections.end(),
                                    BySectionInfo( sectionInfo ) );
                if( it == parentNode.childSections.end() ) {
                    node = std::make_shared<SectionNode>( incompleteStats );
                    parentNode.childSections.push_back( node );
                }
                else
                    node = *it;
            }
            m_sectionStack.push_back( node );
            m_deepestSection = std::move(node);
        }

        void assertionStarting(AssertionInfo const&) override {}

        bool assertionEnded(AssertionStats const& assertionStats) override {
            assert(!m_sectionStack.empty());
            // AssertionResult holds a pointer to a temporary DecomposedExpression,
            // which getExpandedExpression() calls to build the expression string.
            // Our section stack copy of the assertionResult will likely outlive the
            // temporary, so it must be expanded or discarded now to avoid calling
            // a destroyed object later.
            prepareExpandedExpression(const_cast<AssertionResult&>( assertionStats.assertionResult ) );
            SectionNode& sectionNode = *m_sectionStack.back();
            sectionNode.assertions.push_back(assertionStats);
            return true;
        }
        void sectionEnded(SectionStats const& sectionStats) override {
            assert(!m_sectionStack.empty());
            SectionNode& node = *m_sectionStack.back();
            node.stats = sectionStats;
            m_sectionStack.pop_back();
        }
        void testCaseEnded(TestCaseStats const& testCaseStats) override {
            auto node = std::make_shared<TestCaseNode>(testCaseStats);
            assert(m_sectionStack.size() == 0);
            node->children.push_back(m_rootSection);
            m_testCases.push_back(node);
            m_rootSection.reset();
        
            assert(m_deepestSection);
            m_deepestSection->stdOut = testCaseStats.stdOut;
            m_deepestSection->stdErr = testCaseStats.stdErr;
        }
        void testGroupEnded(TestGroupStats const& testGroupStats) override {
            auto node = std::make_shared<TestGroupNode>(testGroupStats);
            node->children.swap(m_testCases);
            m_testGroups.push_back(node);
        }
        void testRunEnded(TestRunStats const& testRunStats) override {
            auto node = std::make_shared<TestRunNode>(testRunStats);
            node->children.swap(m_testGroups);
            m_testRuns.push_back(node);
            testRunEndedCumulative();
        }
        virtual void testRunEndedCumulative() = 0;

        void skipTest(TestCaseInfo const&) override {}

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


    struct TestEventListenerBase : StreamingReporterBase<TestEventListenerBase> {
        TestEventListenerBase( ReporterConfig const& _config );

        void assertionStarting(AssertionInfo const&) override;
        bool assertionEnded(AssertionStats const&) override;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_BASES_HPP_INCLUDED
