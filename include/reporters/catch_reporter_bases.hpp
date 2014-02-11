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

namespace Catch {

    struct StreamingReporterBase : SharedImpl<IStreamingReporter> {

        StreamingReporterBase( ReporterConfig const& _config )
        :   m_config( _config.fullConfig() ),
            stream( _config.stream() )
        {}

        virtual ~StreamingReporterBase();

        virtual void noMatchingTestCases( std::string const& ) {}

        virtual void testRunStarting( TestRunInfo const& _testRunInfo ) {
            currentTestRunInfo = _testRunInfo;
        }
        virtual void testGroupStarting( GroupInfo const& _groupInfo ) {
            currentGroupInfo = _groupInfo;
        }

        virtual void testCaseStarting( TestCaseInfo const& _testInfo ) {
            currentTestCaseInfo = _testInfo;
        }
        virtual void sectionStarting( SectionInfo const& _sectionInfo ) {
            m_sectionStack.push_back( _sectionInfo );
        }

        virtual void sectionEnded( SectionStats const& /* _sectionStats */ ) {
            m_sectionStack.pop_back();
        }
        virtual void testCaseEnded( TestCaseStats const& /* _testCaseStats */ ) {
            currentTestCaseInfo.reset();
            assert( m_sectionStack.empty() );
        }
        virtual void testGroupEnded( TestGroupStats const& /* _testGroupStats */ ) {
            currentGroupInfo.reset();
        }
        virtual void testRunEnded( TestRunStats const& /* _testRunStats */ ) {
            currentTestCaseInfo.reset();
            currentGroupInfo.reset();
            currentTestRunInfo.reset();
        }

        Ptr<IConfig> m_config;
        std::ostream& stream;

        LazyStat<TestRunInfo> currentTestRunInfo;
        LazyStat<GroupInfo> currentGroupInfo;
        LazyStat<TestCaseInfo> currentTestCaseInfo;

        std::vector<SectionInfo> m_sectionStack;
    };

    struct CumulativeReporterBase : SharedImpl<IStreamingReporter> {
        template<typename T, typename ChildNodeT>
        struct Node : SharedImpl<> {
            explicit Node( T const& _value ) : value( _value ) {}
            virtual ~Node() {}

            typedef std::vector<Ptr<ChildNodeT> > ChildNodes;
            T value;
            ChildNodes children;
        };
        struct SectionNode : SharedImpl<> {
            explicit SectionNode( SectionStats const& _stats ) : stats( _stats ) {}
            virtual ~SectionNode();

            bool operator == ( SectionNode const& other ) const {
                return stats.sectionInfo.lineInfo == other.stats.sectionInfo.lineInfo;
            }
            bool operator == ( Ptr<SectionNode> const& other ) const {
                return operator==( *other );
            }

            SectionStats stats;
            typedef std::vector<Ptr<SectionNode> > ChildSections;
            typedef std::vector<AssertionStats> Assertions;
            ChildSections childSections;
            Assertions assertions;
            std::string stdOut;
            std::string stdErr;
        };

        struct BySectionInfo {
            BySectionInfo( SectionInfo const& other ) : m_other( other ) {}
            bool operator() ( Ptr<SectionNode> const& node ) const {
                return node->stats.sectionInfo.lineInfo == m_other.lineInfo;
            }
        private:
            BySectionInfo& operator=( BySectionInfo const& other ); // = delete;

            SectionInfo const& m_other;
        };


        typedef Node<TestCaseStats, SectionNode> TestCaseNode;
        typedef Node<TestGroupStats, TestCaseNode> TestGroupNode;
        typedef Node<TestRunStats, TestGroupNode> TestRunNode;

        CumulativeReporterBase( ReporterConfig const& _config )
        :   m_config( _config.fullConfig() ),
            stream( _config.stream() )
        {}
        ~CumulativeReporterBase();

        virtual void testRunStarting( TestRunInfo const& ) {}
        virtual void testGroupStarting( GroupInfo const& ) {}

        virtual void testCaseStarting( TestCaseInfo const& ) {}

        virtual void sectionStarting( SectionInfo const& sectionInfo ) {
            SectionStats incompleteStats( sectionInfo, Counts(), 0, false );
            Ptr<SectionNode> node;
            if( m_sectionStack.empty() ) {
                if( !m_rootSection )
                    m_rootSection = new SectionNode( incompleteStats );
                node = m_rootSection;
            }
            else {
                SectionNode& parentNode = *m_sectionStack.back();
                SectionNode::ChildSections::const_iterator it =
                    std::find_if(   parentNode.childSections.begin(),
                                    parentNode.childSections.end(),
                                    BySectionInfo( sectionInfo ) );
                if( it == parentNode.childSections.end() ) {
                    node = new SectionNode( incompleteStats );
                    parentNode.childSections.push_back( node );
                }
                else
                    node = *it;
            }
            m_sectionStack.push_back( node );
            m_deepestSection = node;
        }

        virtual void assertionStarting( AssertionInfo const& ) {}

        virtual bool assertionEnded( AssertionStats const& assertionStats ) {
            assert( !m_sectionStack.empty() );
            SectionNode& sectionNode = *m_sectionStack.back();
            sectionNode.assertions.push_back( assertionStats );
            return true;
        }
        virtual void sectionEnded( SectionStats const& sectionStats ) {
            assert( !m_sectionStack.empty() );
            SectionNode& node = *m_sectionStack.back();
            node.stats = sectionStats;
            m_sectionStack.pop_back();
        }
        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) {
            Ptr<TestCaseNode> node = new TestCaseNode( testCaseStats );
            assert( m_sectionStack.size() == 0 );
            node->children.push_back( m_rootSection );
            m_testCases.push_back( node );
            m_rootSection.reset();

            assert( m_deepestSection );
            m_deepestSection->stdOut = testCaseStats.stdOut;
            m_deepestSection->stdErr = testCaseStats.stdErr;
        }
        virtual void testGroupEnded( TestGroupStats const& testGroupStats ) {
            Ptr<TestGroupNode> node = new TestGroupNode( testGroupStats );
            node->children.swap( m_testCases );
            m_testGroups.push_back( node );
        }
        virtual void testRunEnded( TestRunStats const& testRunStats ) {
            Ptr<TestRunNode> node = new TestRunNode( testRunStats );
            node->children.swap( m_testGroups );
            m_testRuns.push_back( node );
            testRunEndedCumulative();
        }
        virtual void testRunEndedCumulative() = 0;

        Ptr<IConfig> m_config;
        std::ostream& stream;
        std::vector<AssertionStats> m_assertions;
        std::vector<std::vector<Ptr<SectionNode> > > m_sections;
        std::vector<Ptr<TestCaseNode> > m_testCases;
        std::vector<Ptr<TestGroupNode> > m_testGroups;

        std::vector<Ptr<TestRunNode> > m_testRuns;

        Ptr<SectionNode> m_rootSection;
        Ptr<SectionNode> m_deepestSection;
        std::vector<Ptr<SectionNode> > m_sectionStack;

    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_BASES_HPP_INCLUDED
