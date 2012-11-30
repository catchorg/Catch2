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
    struct ReporterConfig {
        ReporterConfig( std::ostream& _stream, ConfigData const& _fullConfig )
        :   m_stream( &_stream ), m_fullConfig( _fullConfig ) {}

        std::ostream& stream()                  { return *m_stream; }
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

    struct AssertionStats : SharedImpl<> {
        AssertionStats( AssertionResult const& _assertionResult,
                        Totals const& _totals )
        :   assertionResult( _assertionResult ),
            totals( _totals )
        {}
        virtual ~AssertionStats();
        
        AssertionResult assertionResult;
        Totals totals;
    };

    struct SectionStats : SharedImpl<> {
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

    struct TestCaseStats : SharedImpl<> {
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
    
    struct TestGroupStats : SharedImpl<> {
        TestGroupStats( std::string const& _groupName,
                        Totals const& _totals,
                        bool _aborting )
        :   groupName( _groupName ),
            totals( _totals ),
            aborting( _aborting )
        {}
        virtual ~TestGroupStats();

        std::string groupName;
        Totals totals;
        bool aborting;
    };
    
    struct TestRunStats : SharedImpl<> {
        TestRunStats(   std::string const& _runName,
                        Totals const& _totals,
                        bool _aborting )
        :   runName( _runName ),
            totals( _totals ),
            aborting( _aborting )
        {}
        virtual ~TestRunStats();
        
        std::string runName;
        Totals totals;
        bool aborting;
    };

    // !Work In progress
    struct IStreamingReporter : IShared {
        virtual ~IStreamingReporter();
        virtual ReporterPreferences getPreferences() const = 0;

        virtual void testRunStarting( std::string const& runName ) = 0;
        virtual void testGroupStarting( std::string const& groupName ) = 0;

        virtual void testCaseStarting( TestCaseInfo const& testInfo ) = 0;
        virtual void sectionStarting( SectionInfo const& sectionInfo ) = 0;

        virtual void assertionStarting( AssertionInfo const& assertionInfo ) = 0;

        virtual void assertionEnded( Ptr<AssertionStats const> const& assertionStats ) = 0;
        virtual void sectionEnded( Ptr<SectionStats const> const& sectionStats ) = 0;
        virtual void testCaseEnded( Ptr<TestCaseStats const> const& testCaseStats ) = 0;
        virtual void testGroupEnded( Ptr<TestGroupStats const> const& testGroupStats ) = 0;
        virtual void testRunEnded( Ptr<TestRunStats const> const& testRunStats ) = 0;
    };
    // !TBD: Derived helper that implements the streaming interface but holds the stats
    // - declares a new interface where methods are called at the end of each event
    // - this would be used by the JUnit reporter, for example.
    // - it may be used by the basic reporter, too, but that would clear down the stack
    //   as it goes


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

        virtual void testRunStarting( std::string const& ) {
            m_legacyReporter->StartTesting();
        }
        virtual void testGroupStarting( std::string const& groupName ) {
            m_legacyReporter->StartGroup( groupName );
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

        virtual void assertionEnded( Ptr<AssertionStats const> const& assertionStats ) {
            m_legacyReporter->Result( assertionStats->assertionResult );
        }
        virtual void sectionEnded( Ptr<SectionStats const> const& sectionStats ) {
            if( sectionStats->missingAssertions )
                m_legacyReporter->NoAssertionsInSection( sectionStats->sectionInfo.name );
            m_legacyReporter->EndSection( sectionStats->sectionInfo.name, sectionStats->assertions );
        }
        virtual void testCaseEnded( Ptr<TestCaseStats const> const& testCaseStats ) {
            if( testCaseStats->missingAssertions )
                m_legacyReporter->NoAssertionsInTestCase( testCaseStats->testInfo.name );
            m_legacyReporter->EndTestCase
                (   testCaseStats->testInfo,
                    testCaseStats->totals,
                    testCaseStats->stdOut,
                    testCaseStats->stdErr );
        }
        virtual void testGroupEnded( Ptr<TestGroupStats const> const& testGroupStats ) {
            if( testGroupStats->aborting )
                m_legacyReporter->Aborted();
            m_legacyReporter->EndGroup( testGroupStats->groupName, testGroupStats->totals );
        }
        virtual void testRunEnded( Ptr<TestRunStats const> const& testRunStats ) {
            m_legacyReporter->EndTesting( testRunStats->totals );
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
