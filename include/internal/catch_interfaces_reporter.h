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

#include <string>
#include <ostream>
#include <map>

namespace Catch
{
    struct ReporterConfig
    {
        ReporterConfig( std::ostream& _stream, const ConfigData& _fullConfig )
        :   m_stream( &_stream ), m_fullConfig( _fullConfig ) {}

        std::ostream& stream()                  { return *m_stream; }
        std::string name() const                { return m_fullConfig.name; }
        bool includeSuccessfulResults() const   { return m_fullConfig.includeWhichResults == Include::SuccessfulResults; }
        bool warnAboutMissingAssertions() const { return m_fullConfig.warnings & ConfigData::WarnAbout::NoAssertions; }

    private:
        std::ostream* m_stream;
        ConfigData m_fullConfig;
    };

    struct ReporterPreferences
    {
        ReporterPreferences()
        : shouldRedirectStdOut( false )
        {}

        bool shouldRedirectStdOut;
    };

    struct AssertionStats {
        AssertionStats( const AssertionResult& _assertionResult,
                        const Totals& _totals )
        :   assertionResult( _assertionResult ),
            totals( _totals )
        {}
        
//        AssertionInfo assertionInfo; // !TBD: needed? It's in the result
        AssertionResult assertionResult;
        Totals totals;
    };

    struct TestCaseStats {
        TestCaseStats(  const TestCaseInfo& _testInfo,
                        const Totals& _totals,
                        const std::string& _stdOut,
                        const std::string& _stdErr,
                        bool _missingAssertions,
                        bool _aborting )
        : testInfo( _testInfo ),
            totals( _totals ),
            stdOut( _stdOut ),
            stdErr( _stdErr ),
            missingAssertions( _missingAssertions ),
            aborting( _aborting )
        {}

        TestCaseInfo testInfo;
        Totals totals;
        std::string stdOut;
        std::string stdErr;
        bool missingAssertions;
        bool aborting;
    };
    
    struct TestGroupStats {
        TestGroupStats( const std::string& _groupName,
                        const Totals& _totals,
                        bool _aborting )
        :   groupName( _groupName ),
            totals( _totals ),
            aborting( _aborting )
        {}

        std::string groupName;
        Totals totals;
        bool aborting;
    };
    
    struct TestRunStats {
        TestRunStats(   const std::string& _runName,
                        const Totals& _totals,
                        bool _aborting )
        :   runName( _runName ),
            totals( _totals ),
            aborting( _aborting )
        {}
        
        std::string runName;
        Totals totals;
        bool aborting;
    };

    // !Work In progress
    struct IStreamingReporter : IShared {
        virtual ~IStreamingReporter();
        virtual ReporterPreferences getPreferences() const = 0;

        virtual void testRunStarting( const std::string& runName ) = 0;
        virtual void testGroupStarting( const std::string& groupName ) = 0;

        // !TBD: include section info (perhaps TestCase has an isSection flag and/ or a parent pointer
        virtual void testCaseStarting( const TestCaseInfo& testInfo ) = 0;
        virtual void assertionStarting( const AssertionInfo& assertionInfo ) = 0;

        virtual void assertionEnding( const AssertionStats& assertionStats ) = 0;
        virtual void testCaseEnding( const TestCaseStats& testCaseStats ) = 0;
        virtual void testGroupEnding( const TestGroupStats& testGroupStats ) = 0;
        virtual void testRunEnding( const TestRunStats& testRunStats ) = 0;
    };
    // !TBD: Derived helper that implements the streaming interface but holds the stats
    // - declares a new interface where methods are called at the end of each event
    // - this would be used by the JUnit reporter, for example.
    // - it may be used by the basic reporter, too, but that would clear down the stack
    //   as it goes


    struct IReporter : IShared {
        virtual ~IReporter();

        virtual bool shouldRedirectStdout() const = 0;

        virtual void StartTesting() = 0;        
        virtual void EndTesting( const Totals& totals ) = 0;        

        virtual void StartGroup( const std::string& groupName ) = 0;
        virtual void EndGroup( const std::string& groupName, const Totals& totals ) = 0;        

        virtual void StartTestCase( const TestCaseInfo& testInfo ) = 0;
        // TestCaseResult
        virtual void EndTestCase( const TestCaseInfo& testInfo, const Totals& totals, const std::string& stdOut, const std::string& stdErr ) = 0;

        // SectionInfo
        virtual void StartSection( const std::string& sectionName, const std::string& description ) = 0;
        // Section Result
        virtual void EndSection( const std::string& sectionName, const Counts& assertions ) = 0;

        // - merge into SectionResult ?
        virtual void NoAssertionsInSection( const std::string& sectionName ) = 0;
        virtual void NoAssertionsInTestCase( const std::string& testName ) = 0;

        // - merge into SectionResult, TestCaseResult, GroupResult & TestRunResult
        virtual void Aborted() = 0;

        // AssertionReslt
        virtual void Result( const AssertionResult& result ) = 0;
    };

    class LegacyReporterAdapter : public SharedImpl<IStreamingReporter>
    {
    public:
        LegacyReporterAdapter( const Ptr<IReporter>& legacyReporter, const ReporterConfig& config )
        :   m_legacyReporter( legacyReporter ),
            m_config( config )
        {}
        virtual ~LegacyReporterAdapter();

        virtual ReporterPreferences getPreferences() const {
            ReporterPreferences prefs;
            prefs.shouldRedirectStdOut = m_legacyReporter->shouldRedirectStdout();
            return prefs;
        }

        virtual void testRunStarting( const std::string& ) {
            m_legacyReporter->StartTesting();
        }
        virtual void testGroupStarting( const std::string& groupName ) {
            m_legacyReporter->StartGroup( groupName );
        }
        virtual void testCaseStarting( const TestCaseInfo& testInfo ) {
            m_legacyReporter->StartTestCase( testInfo );
        }
        virtual void assertionStarting( const AssertionInfo& ) {
            // Not on legacy interface
        }

        virtual void assertionEnding( const AssertionStats& assertionStats ) {
            m_legacyReporter->Result( assertionStats.assertionResult );
        }
        virtual void testCaseEnding( const TestCaseStats& testCaseStats ) {
            if( testCaseStats.missingAssertions )
                m_legacyReporter->NoAssertionsInTestCase( testCaseStats.testInfo.name );
            m_legacyReporter->EndTestCase( testCaseStats.testInfo, testCaseStats.totals, testCaseStats.stdOut, testCaseStats.stdErr );
        }
        virtual void testGroupEnding( const TestGroupStats& testGroupStats ) {
            if( testGroupStats.aborting )
                m_legacyReporter->Aborted();
            m_legacyReporter->EndGroup( testGroupStats.groupName, testGroupStats.totals );
        }
        virtual void testRunEnding( const TestRunStats& testRunStats ) {
            m_legacyReporter->EndTesting( testRunStats.totals );
        }

    private:
        Ptr<IReporter> m_legacyReporter;
        ReporterConfig m_config;
    };

    
    struct IReporterFactory {
        virtual ~IReporterFactory();
        virtual IReporter* create( const ReporterConfig& config ) const = 0;
        virtual std::string getDescription() const = 0;
    };

    struct IReporterRegistry {
        typedef std::map<std::string, IReporterFactory*> FactoryMap;

        virtual ~IReporterRegistry();
        virtual IReporter* create( const std::string& name, const ReporterConfig& config ) const = 0;        
        virtual const FactoryMap& getFactories() const = 0;
    };
    
    inline std::string trim( const std::string& str ) {
        std::string::size_type start = str.find_first_not_of( "\n\r\t " );
        std::string::size_type end = str.find_last_not_of( "\n\r\t " );
        
        return start != std::string::npos ? str.substr( start, 1+end-start ) : "";
    }
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_REPORTER_H_INCLUDED
