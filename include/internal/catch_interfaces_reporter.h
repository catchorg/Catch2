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
#include "catch_option.hpp"

#include <string>
#include <ostream>
#include <map>

namespace Catch
{
    struct ReporterConfig {
        ReporterConfig( std::ostream& _stream, ConfigData const& _fullConfig )
        :   m_stream( &_stream ), m_fullConfig( _fullConfig ) {}

        std::ostream& stream() const            { return *m_stream; }
        std::string name() const                { return m_fullConfig.name; }
        bool includeSuccessfulResults() const   { return m_fullConfig.includeWhichResults == Include::SuccessfulResults; }
        bool warnAboutMissingAssertions() const { return m_fullConfig.warnings & ConfigData::WarnAbout::NoAssertions; }

    private:
        std::ostream* m_stream;
        ConfigData m_fullConfig;
    };

    struct ReporterPreferences {
        ReporterPreferences()
        : shouldRedirectStdOut( false )
        {}

        bool shouldRedirectStdOut;
    };

    struct TestRunInfo {
        TestRunInfo( std::string const& _name ) : name( _name ) {}
        std::string name;
    };
    struct GroupInfo {
        GroupInfo( std::string const& _name ) : name( _name ) {}
        std::string name;
    };

    struct SectionInfo {
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
        ThreadedSectionInfo( SectionInfo const& _sectionInfo, Ptr<ThreadedSectionInfo> const& _parent = Ptr<ThreadedSectionInfo>() )
        :   SectionInfo( _sectionInfo ),
            parent( _parent ),
            printed( false )
        {}
        virtual ~ThreadedSectionInfo();

        std::vector<Ptr<ThreadedSectionInfo> > children;
        Ptr<ThreadedSectionInfo> parent;
        bool printed;
    };

    struct AssertionStats {
        AssertionStats( AssertionResult const& _assertionResult,
                        Totals const& _totals )
        :   assertionResult( _assertionResult ),
            totals( _totals )
        {}
        virtual ~AssertionStats();
        
        AssertionResult assertionResult;
        Totals totals;
    };

    struct SectionStats {
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
        TestCaseStats(  TestCaseInfo const& _testInfo,
                        Totals const& _totals,
                        std::string const& _stdOut,
                        std::string const& _stdErr,
                        bool _missingAssertions,
                        bool _aborting )
        : testInfo( _testInfo ),
            totals( _totals ),
            stdOut( _stdOut ),
            stdErr( _stdErr ),
            missingAssertions( _missingAssertions ),
            aborting( _aborting )
        {}
        virtual ~TestCaseStats();

        TestCaseInfo testInfo;
        Totals totals;
        std::string stdOut;
        std::string stdErr;
        bool missingAssertions;
        bool aborting;
    };
    
    struct TestGroupStats {
        TestGroupStats( GroupInfo const& _groupInfo,
                        Totals const& _totals,
                        bool _aborting )
        :   groupInfo( _groupInfo ),
            totals( _totals ),
            aborting( _aborting )
        {}
        TestGroupStats( GroupInfo const& _groupInfo )
        :   groupInfo( _groupInfo ),
            aborting( false )
        {}
        virtual ~TestGroupStats();

        GroupInfo groupInfo;
        Totals totals;
        bool aborting;
    };
    
    struct TestRunStats {
        TestRunStats(   TestRunInfo const& _runInfo,
                        Totals const& _totals,
                        bool _aborting )
        :   runInfo( _runInfo ),
            totals( _totals ),
            aborting( _aborting )
        {}
        TestRunStats( TestRunStats const& _other )
        :   runInfo( _other.runInfo ),
            totals( _other.totals ),
            aborting( _other.aborting )
        {}
        virtual ~TestRunStats();

        TestRunInfo runInfo;
        Totals totals;
        bool aborting;
    };

    // !Work In progress
    struct IStreamingReporter : IShared {
        virtual ~IStreamingReporter();

        // Implementing class must also provide the following static methid:
        // static std::string getDescription();

        virtual ReporterPreferences getPreferences() const = 0;

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
        :   m_config( _config ),
            stream( _config.stream() )
        {}

        virtual ~StreamingReporterBase();

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
            unusedSectionInfo = sectionInfo;
            if( !currentSectionInfo ) {
                currentSectionInfo = sectionInfo;
            }
            else {
                currentSectionInfo->children.push_back( sectionInfo );
                sectionInfo->parent = currentSectionInfo;
                currentSectionInfo = sectionInfo;
            }
        }

        virtual void sectionEnded( SectionStats const& /* _sectionStats */ ) {
            currentSectionInfo = currentSectionInfo->parent;
            unusedSectionInfo = currentSectionInfo;
        }
        virtual void testCaseEnded( TestCaseStats const& /* _testCaseStats */ ) {
            unusedTestCaseInfo.reset();
        }
        virtual void testGroupEnded( TestGroupStats const& /* _testGroupStats */ ) {
            unusedGroupInfo.reset();
        }
        virtual void testRunEnded( TestRunStats const& /* _testRunStats */ ) {
            currentSectionInfo.reset();
            unusedSectionInfo.reset();
            unusedTestCaseInfo.reset();
            unusedGroupInfo.reset();
            testRunInfo.reset();
        }

        ReporterConfig m_config;
        Option<TestRunInfo> testRunInfo;
        Option<GroupInfo> unusedGroupInfo;
        Option<TestCaseInfo> unusedTestCaseInfo;
        Ptr<ThreadedSectionInfo> unusedSectionInfo;
        Ptr<ThreadedSectionInfo> currentSectionInfo;
        bool currentSectionOpen;
        std::ostream& stream;
    };

    struct TestGroupNode : TestGroupStats {
        TestGroupNode( TestGroupStats const& _stats ) : TestGroupStats( _stats ) {}
//        TestGroupNode( GroupInfo const& _info ) : TestGroupStats( _stats ) {}
        ~TestGroupNode();

    };

    struct TestRunNode : TestRunStats {

        TestRunNode( TestRunStats const& _stats ) : TestRunStats( _stats ) {}
        ~TestRunNode();
        
        std::vector<TestGroupNode> groups;
    };
    


    // !TBD: Derived helper that implements the streaming interface but holds the stats
    // - declares a new interface where methods are called at the end of each event
    // - this would be used by the JUnit reporter, for example.
    // - it may be used by the basic reporter, too, but that would clear down the stack
    //   as it goes
    struct CumulativeReporterBase : SharedImpl<IStreamingReporter> {

        CumulativeReporterBase( ReporterConfig const& _config )
        :   m_config( _config ),
            stream( _config.stream() )
        {}

        virtual ~CumulativeReporterBase();

        virtual void testRunStarting( TestRunInfo const& _testRunInfo ) {
//            testRunInfo = _testRunInfo;
        }
        virtual void testGroupStarting( GroupInfo const& _groupInfo ) {
            testGroupNode = TestGroupNode( _groupInfo );
        }

        virtual void testCaseStarting( TestCaseInfo const& _testInfo ) {
//            unusedTestCaseInfo = _testInfo;
        }
        virtual void sectionStarting( SectionInfo const& _sectionInfo ) {
//            Ptr<ThreadedSectionInfo> sectionInfo = new ThreadedSectionInfo( _sectionInfo );
//            unusedSectionInfo = sectionInfo;
//            if( !currentSectionInfo ) {
//                currentSectionInfo = sectionInfo;
//            }
//            else {
//                currentSectionInfo->children.push_back( sectionInfo );
//                sectionInfo->parent = currentSectionInfo;
//                currentSectionInfo = sectionInfo;
//            }
        }

        virtual void sectionEnded( SectionStats const& /* _sectionStats */ ) {
//            currentSectionInfo = currentSectionInfo->parent;
//            unusedSectionInfo = currentSectionInfo;
        }
        virtual void testCaseEnded( TestCaseStats const& /* _testCaseStats */ ) {
//            unusedTestCaseInfo.reset();
        }
        virtual void testGroupEnded( TestGroupStats const&  _testGroupStats ) {
//            testGroupNode-> // populate
//            Ptr<TestGroupNode> node ( new TestGroupNode( _testGroupStats ) );
//            unusedGroupInfo.reset();
        }
        virtual void testRunEnded( TestRunStats const& /* _testRunStats */ ) {
//            currentSectionInfo.reset();
//            unusedSectionInfo.reset();
//            unusedTestCaseInfo.reset();
//            unusedGroupInfo.reset();
//            testRunInfo.reset();
        }

        ReporterConfig m_config;
//        Option<TestRunInfo> testRunInfo;
//        Option<GroupInfo> unusedGroupInfo;
//        Option<TestCaseInfo> unusedTestCaseInfo;
//        Ptr<ThreadedSectionInfo> unusedSectionInfo;
//        Ptr<ThreadedSectionInfo> currentSectionInfo;
//        bool currentSectionOpen;
//        Ptr<TestGroupNode> testGroupNode;
        Option<TestGroupNode> testGroupNode;
        std::ostream& stream;
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
        virtual void EndSection( std::string const& sectionName, const Counts& assertions ) = 0;
        virtual void NoAssertionsInSection( std::string const& sectionName ) = 0;
        virtual void NoAssertionsInTestCase( std::string const& testName ) = 0;
        virtual void Aborted() = 0;
        virtual void Result( const AssertionResult& result ) = 0;
    };

    class LegacyReporterAdapter : public SharedImpl<IStreamingReporter>
    {
    public:
        LegacyReporterAdapter( Ptr<IReporter> const& legacyReporter, ReporterConfig const& config )
        :   m_legacyReporter( legacyReporter ),
            m_config( config )
        {}
        virtual ~LegacyReporterAdapter();

        virtual ReporterPreferences getPreferences() const {
            ReporterPreferences prefs;
            prefs.shouldRedirectStdOut = m_legacyReporter->shouldRedirectStdout();
            return prefs;
        }

        virtual void testRunStarting( TestRunInfo const& ) {
            m_legacyReporter->StartTesting();
        }
        virtual void testGroupStarting( GroupInfo const& groupInfo ) {
            m_legacyReporter->StartGroup( groupInfo.name );
        }
        virtual void testCaseStarting( TestCaseInfo const& testInfo ) {
            m_legacyReporter->StartTestCase( testInfo );
        }
        virtual void sectionStarting( SectionInfo const& sectionInfo ) {
            m_legacyReporter->StartSection( sectionInfo.name, sectionInfo.description );
        }
        virtual void assertionStarting( AssertionInfo const& ) {
            // Not on legacy interface
        }

        virtual void assertionEnded( AssertionStats const& assertionStats ) {
            m_legacyReporter->Result( assertionStats.assertionResult );
        }
        virtual void sectionEnded( SectionStats const& sectionStats ) {
            if( sectionStats.missingAssertions )
                m_legacyReporter->NoAssertionsInSection( sectionStats.sectionInfo.name );
            m_legacyReporter->EndSection( sectionStats.sectionInfo.name, sectionStats.assertions );
        }
        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) {
            if( testCaseStats.missingAssertions )
                m_legacyReporter->NoAssertionsInTestCase( testCaseStats.testInfo.name );
            m_legacyReporter->EndTestCase
                (   testCaseStats.testInfo,
                    testCaseStats.totals,
                    testCaseStats.stdOut,
                    testCaseStats.stdErr );
        }
        virtual void testGroupEnded( TestGroupStats const& testGroupStats ) {
            if( testGroupStats.aborting )
                m_legacyReporter->Aborted();
            m_legacyReporter->EndGroup( testGroupStats.groupInfo.name, testGroupStats.totals );
        }
        virtual void testRunEnded( TestRunStats const& testRunStats ) {
            m_legacyReporter->EndTesting( testRunStats.totals );
        }

    private:
        Ptr<IReporter> m_legacyReporter;
        ReporterConfig m_config;
    };

    
    struct IReporterFactory {
        virtual ~IReporterFactory();
        virtual IStreamingReporter* create( ReporterConfig const& config ) const = 0;
        virtual std::string getDescription() const = 0;
    };

    struct IReporterRegistry {
        typedef std::map<std::string, IReporterFactory*> FactoryMap;

        virtual ~IReporterRegistry();
        virtual IStreamingReporter* create( std::string const& name, ReporterConfig const& config ) const = 0;        
        virtual const FactoryMap& getFactories() const = 0;
    };
    
    inline std::string trim( std::string const& str ) {
        std::string::size_type start = str.find_first_not_of( "\n\r\t " );
        std::string::size_type end = str.find_last_not_of( "\n\r\t " );
        
        return start != std::string::npos ? str.substr( start, 1+end-start ) : "";
    }
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_REPORTER_H_INCLUDED
