/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_INTERFACES_REPORTER_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACES_REPORTER_H_INCLUDED

#include "catch_common.h"
#include "catch_totals.hpp"
#include "catch_ptr.hpp"
#include "catch_config.hpp"
#include "catch_test_case_info.h"
#include "catch_assertionresult.h"
#include "catch_message.h"
#include "catch_option.hpp"

#include <cassert>
#include <string>
#include <ostream>
#include <map>

namespace Catch
{
    struct ReporterConfig {
        explicit ReporterConfig( Ptr<IConfig> const& _fullConfig )
        :   m_stream( &_fullConfig->stream() ), m_fullConfig( _fullConfig ) {}

        ReporterConfig( Ptr<IConfig> const& _fullConfig, std::ostream& _stream )
        :   m_stream( &_stream ), m_fullConfig( _fullConfig ) {}

        std::ostream& stream() const    { return *m_stream; }
        Ptr<IConfig> fullConfig() const { return m_fullConfig; }

    private:
        std::ostream* m_stream;
        Ptr<IConfig> m_fullConfig;
    };

    struct ReporterPreferences {
        ReporterPreferences()
        : shouldRedirectStdOut( false )
        {}

        bool shouldRedirectStdOut;
    };

    struct TestRunInfo {
        TestRunInfo( std::string const& _name = "" ) : name( _name ) {}
        std::string name;
    };
    struct GroupInfo {
        GroupInfo() : groupIndex( 0 ), groupsCounts( 0 ) {}
        GroupInfo(  std::string const& _name,
                    std::size_t _groupIndex,
                    std::size_t _groupsCount )
        :   name( _name ),
            groupIndex( _groupIndex ),
            groupsCounts( _groupsCount )
        {}

        std::string name;
        std::size_t groupIndex;
        std::size_t groupsCounts;
    };

    struct SectionInfo {
        SectionInfo() {}
        SectionInfo(    std::string const& _name,
                        std::string const& _description,
                        SourceLineInfo const& _lineInfo )
        :   name( _name ),
            description( _description ),
            lineInfo( _lineInfo )
        {}

        std::string name;
        std::string description;
        SourceLineInfo lineInfo;
    };

    struct ThreadedSectionInfo : SectionInfo, SharedImpl<> {
        ThreadedSectionInfo( SectionInfo const& _sectionInfo, ThreadedSectionInfo* _parent = NULL )
        :   SectionInfo( _sectionInfo ),
            parent( _parent )
        {}
        virtual ~ThreadedSectionInfo();

        std::vector<Ptr<ThreadedSectionInfo> > children;
        ThreadedSectionInfo* parent;
    };

    struct AssertionStats {
        AssertionStats( AssertionResult const& _assertionResult,
                        std::vector<MessageInfo> const& _infoMessages,
                        Totals const& _totals )
        :   assertionResult( _assertionResult ),
            infoMessages( _infoMessages ),
            totals( _totals )
        {
            if( assertionResult.hasMessage() ) {
                // Copy message into messages list.
                // !TBD This should have been done earlier, somewhere
                MessageBuilder builder( assertionResult.getTestMacroName(), assertionResult.getSourceInfo(), assertionResult.getResultType() );
                builder << assertionResult.getMessage();
                infoMessages.push_back( builder.build() );
            }
        }
        virtual ~AssertionStats();
        
        AssertionResult assertionResult;
        std::vector<MessageInfo> infoMessages;
        Totals totals;
    };

    struct SectionStats {
        SectionStats() : missingAssertions( false ) {}
        SectionStats(   SectionInfo const& _sectionInfo,
                        Counts const& _assertions,
                        bool _missingAssertions )
        :   sectionInfo( _sectionInfo ),
            assertions( _assertions ),
            missingAssertions( _missingAssertions )
        {}
        virtual ~SectionStats();

        SectionInfo sectionInfo;
        Counts assertions;
        bool missingAssertions;
    };

    struct TestCaseStats {
        TestCaseStats()
        : missingAssertions( false ),
            aborting( false)
        {}
        TestCaseStats(  TestCaseInfo const& _testInfo,
                        Totals const& _totals,
                        double _timeSecs,
                        std::string const& _stdOut,
                        std::string const& _stdErr,
                        bool _missingAssertions,
                        bool _aborting )
        : testInfo( _testInfo ),
            totals( _totals ),
            timeSecs( _timeSecs ),
            stdOut( _stdOut ),
            stdErr( _stdErr ),
            missingAssertions( _missingAssertions ),
            aborting( _aborting )
        {}
        virtual ~TestCaseStats();

        TestCaseInfo testInfo;
        Totals totals;
        double timeSecs;
        std::string stdOut;
        std::string stdErr;
        bool missingAssertions;
        bool aborting;
    };
    
    struct TestGroupStats {
        TestGroupStats()
        : aborting( false )
        {}

        TestGroupStats( GroupInfo const& _groupInfo,
                        Totals const& _totals,
                        double _timeSecs,
                        bool _aborting )
        :   groupInfo( _groupInfo ),
            totals( _totals ),
            timeSecs( _timeSecs ),
            aborting( _aborting )
        {}
        TestGroupStats( GroupInfo const& _groupInfo )
        :   groupInfo( _groupInfo ),
            timeSecs( 0.0 ),
            aborting( false )
        {}
        virtual ~TestGroupStats();

        GroupInfo groupInfo;
        Totals totals;
        double timeSecs;
        bool aborting;
    };
    
    struct TestRunStats {
        TestRunStats()
        :   timeSecs( 0.0 ),
            aborting( false )
        {}
        TestRunStats(   TestRunInfo const& _runInfo,
                        Totals const& _totals,
                        double _timeSecs,
                        bool _aborting )
        :   runInfo( _runInfo ),
            totals( _totals ),
            timeSecs( _timeSecs ),
            aborting( _aborting )
        {}
        virtual ~TestRunStats();

        TestRunInfo runInfo;
        Totals totals;
        double timeSecs;
        bool aborting;
    };

    struct IStreamingReporter : IShared {
        virtual ~IStreamingReporter();

        // Implementing class must also provide the following static method:
        // static std::string getDescription();

        virtual ReporterPreferences getPreferences() const = 0;
        
        virtual void noMatchingTestCases( std::string const& spec ) = 0;

        virtual void testRunStarting( TestRunInfo const& testRunInfo ) = 0;
        virtual void testGroupStarting( GroupInfo const& groupInfo ) = 0;

        virtual void testCaseStarting( TestCaseInfo const& testInfo ) = 0;
        virtual void sectionStarting( SectionInfo const& sectionInfo ) = 0;

        virtual void assertionStarting( AssertionInfo const& assertionInfo ) = 0;

        virtual void assertionEnded( AssertionStats const& assertionStats ) = 0;
        virtual void sectionEnded( SectionStats const& sectionStats ) = 0;
        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) = 0;
        virtual void testGroupEnded( TestGroupStats const& testGroupStats ) = 0;
        virtual void testRunEnded( TestRunStats const& testRunStats ) = 0;
    };

    struct StreamingReporterBase : SharedImpl<IStreamingReporter> {

        StreamingReporterBase( ReporterConfig const& _config )
        :   m_config( _config.fullConfig() ),
            stream( _config.stream() )
        {}

        virtual ~StreamingReporterBase();
        
        virtual void noMatchingTestCases( std::string const& ) {}

        virtual void testRunStarting( TestRunInfo const& _testRunInfo ) {
            testRunInfo = _testRunInfo;
        }
        virtual void testGroupStarting( GroupInfo const& _groupInfo ) {
            unusedGroupInfo = _groupInfo;
        }

        virtual void testCaseStarting( TestCaseInfo const& _testInfo ) {
            unusedTestCaseInfo = _testInfo;
        }
        virtual void sectionStarting( SectionInfo const& _sectionInfo ) {
            Ptr<ThreadedSectionInfo> sectionInfo = new ThreadedSectionInfo( _sectionInfo );
            if( !currentSectionInfo ) {
                currentSectionInfo = sectionInfo;
                m_rootSections.push_back( currentSectionInfo );
            }
            else {
                currentSectionInfo->children.push_back( sectionInfo );
                sectionInfo->parent = currentSectionInfo.get();
                currentSectionInfo = sectionInfo;
            }
        }

        virtual void sectionEnded( SectionStats const& /* _sectionStats */ ) {
            currentSectionInfo = currentSectionInfo->parent;
        }
        virtual void testCaseEnded( TestCaseStats const& /* _testCaseStats */ ) {
            unusedTestCaseInfo.reset();
        }
        virtual void testGroupEnded( TestGroupStats const& /* _testGroupStats */ ) {
            unusedGroupInfo.reset();
        }
        virtual void testRunEnded( TestRunStats const& /* _testRunStats */ ) {
            currentSectionInfo.reset();
            unusedTestCaseInfo.reset();
            unusedGroupInfo.reset();
            testRunInfo.reset();
        }

        Ptr<IConfig> m_config;
        Option<TestRunInfo> testRunInfo;
        Option<GroupInfo> unusedGroupInfo;
        Option<TestCaseInfo> unusedTestCaseInfo;
        Ptr<ThreadedSectionInfo> currentSectionInfo;
        std::ostream& stream;
        
        // !TBD: This should really go in the TestCaseStats class
        std::vector<Ptr<ThreadedSectionInfo> > m_rootSections;        
    };

    struct AccumulatingReporterBase : SharedImpl<IStreamingReporter> {
    protected:
        struct AccumTestCaseStats {
            TestCaseStats testCase;
            std::vector<AssertionStats> tests;
        };
        struct AccumTestGroupStats {
            TestGroupStats testGroup;
            std::vector<AccumTestCaseStats> testCases;
        };
        struct AccumTestRunStats {
            TestRunStats testRun;
            std::vector<AccumTestGroupStats> testGroups;
        };

    public:
        virtual ~AccumulatingReporterBase();
        
        virtual void testRunStarting( TestRunInfo const& testRunInfo ) {
            (void)testRunInfo;
            assert( m_accumGroups.empty() );
            assert( m_accumCases.empty() );
            assert( m_accumTests.empty() );
        }
        virtual void testGroupStarting( GroupInfo const& groupInfo ) {
            (void)groupInfo;
            assert( m_accumCases.empty() );
            assert( m_accumTests.empty() );
        }
        virtual void testCaseStarting( TestCaseInfo const& testInfo ) {
            (void)testInfo;
            assert( m_accumTests.empty() );
        }
        virtual void sectionStarting( SectionInfo const& sectionInfo ) {
            (void)sectionInfo;
        }
        virtual void assertionStarting( AssertionInfo const& assertionInfo ) {
            (void)assertionInfo;
        }

        virtual void assertionEnded( AssertionStats const& assertionStats ) {
            m_accumTests.push_back( assertionStats );
        }
        virtual void sectionEnded( SectionStats const& sectionStats ) {
            (void)sectionStats;
        }
        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) {
            AccumTestCaseStats stats;
            stats.testCase = testCaseStats;
            std::swap(stats.tests, m_accumTests);
            m_accumCases.push_back( stats );
        }
        virtual void testGroupEnded( TestGroupStats const& testGroupStats ) {
            assert( m_accumTests.empty() );
            AccumTestGroupStats stats;
            stats.testGroup = testGroupStats;
            std::swap(stats.testCases, m_accumCases);
            m_accumGroups.push_back( stats );
        }
        virtual void testRunEnded( TestRunStats const& testRunStats ) {
            assert( m_accumTests.empty() );
            assert( m_accumCases.empty() );
            AccumTestRunStats stats;
            stats.testRun = testRunStats;
            std::swap(stats.testGroups, m_accumGroups);

            processResults( stats );
        }

        virtual void processResults( AccumTestRunStats const& testRun ) = 0;

    private:
        std::vector<AccumTestGroupStats>   m_accumGroups;
        std::vector<AccumTestCaseStats>    m_accumCases;
        std::vector<AssertionStats>        m_accumTests;
    };

    // Deprecated
    struct IReporter : IShared {
        virtual ~IReporter();

        virtual bool shouldRedirectStdout() const = 0;

        virtual void StartTesting() = 0;        
        virtual void EndTesting( Totals const& totals ) = 0;
        virtual void StartGroup( std::string const& groupName ) = 0;
        virtual void EndGroup( std::string const& groupName, Totals const& totals ) = 0;
        virtual void StartTestCase( TestCaseInfo const& testInfo ) = 0;
        virtual void EndTestCase( TestCaseInfo const& testInfo, Totals const& totals, std::string const& stdOut, std::string const& stdErr ) = 0;
        virtual void StartSection( std::string const& sectionName, std::string const& description ) = 0;
        virtual void EndSection( std::string const& sectionName, Counts const& assertions ) = 0;
        virtual void NoAssertionsInSection( std::string const& sectionName ) = 0;
        virtual void NoAssertionsInTestCase( std::string const& testName ) = 0;
        virtual void Aborted() = 0;
        virtual void Result( AssertionResult const& result ) = 0;
    };

    
    struct IReporterFactory {
        virtual ~IReporterFactory();
        virtual IStreamingReporter* create( ReporterConfig const& config ) const = 0;
        virtual std::string getDescription() const = 0;
    };

    struct IReporterRegistry {
        typedef std::map<std::string, IReporterFactory*> FactoryMap;

        virtual ~IReporterRegistry();
        virtual IStreamingReporter* create( std::string const& name, Ptr<IConfig> const& config ) const = 0;        
        virtual FactoryMap const& getFactories() const = 0;
    };
    
    inline std::string trim( std::string const& str ) {
        const char* const stripAway = "\n\r\t ";
        std::string::size_type start = str.find_first_not_of( stripAway );
        std::string::size_type end = str.find_last_not_of( stripAway );
        
        return start != std::string::npos ? str.substr( start, 1+end-start ) : "";
    }
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_REPORTER_H_INCLUDED
