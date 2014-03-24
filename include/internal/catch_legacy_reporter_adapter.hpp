/*
 *  Created by Phil on 6th April 2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_LEGACY_REPORTER_ADAPTER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_LEGACY_REPORTER_ADAPTER_HPP_INCLUDED

#include "catch_legacy_reporter_adapter.h"

namespace Catch
{
    INTERNAL_CATCH_INLINE LegacyReporterAdapter::LegacyReporterAdapter( Ptr<IReporter> const& legacyReporter )
    :   m_legacyReporter( legacyReporter )
    {}
    INTERNAL_CATCH_INLINE LegacyReporterAdapter::~LegacyReporterAdapter() {}

    INTERNAL_CATCH_INLINE ReporterPreferences LegacyReporterAdapter::getPreferences() const {
        ReporterPreferences prefs;
        prefs.shouldRedirectStdOut = m_legacyReporter->shouldRedirectStdout();
        return prefs;
    }

    INTERNAL_CATCH_INLINE void LegacyReporterAdapter::noMatchingTestCases( std::string const& ) {}
    INTERNAL_CATCH_INLINE void LegacyReporterAdapter::testRunStarting( TestRunInfo const& ) {
        m_legacyReporter->StartTesting();
    }
    INTERNAL_CATCH_INLINE void LegacyReporterAdapter::testGroupStarting( GroupInfo const& groupInfo ) {
        m_legacyReporter->StartGroup( groupInfo.name );
    }
    INTERNAL_CATCH_INLINE void LegacyReporterAdapter::testCaseStarting( TestCaseInfo const& testInfo ) {
        m_legacyReporter->StartTestCase( testInfo );
    }
    INTERNAL_CATCH_INLINE void LegacyReporterAdapter::sectionStarting( SectionInfo const& sectionInfo ) {
        m_legacyReporter->StartSection( sectionInfo.name, sectionInfo.description );
    }
    INTERNAL_CATCH_INLINE void LegacyReporterAdapter::assertionStarting( AssertionInfo const& ) {
        // Not on legacy interface
    }

    INTERNAL_CATCH_INLINE bool LegacyReporterAdapter::assertionEnded( AssertionStats const& assertionStats ) {
        if( assertionStats.assertionResult.getResultType() != ResultWas::Ok ) {
            for( std::vector<MessageInfo>::const_iterator it = assertionStats.infoMessages.begin(), itEnd = assertionStats.infoMessages.end();
                    it != itEnd;
                    ++it ) {
                if( it->type == ResultWas::Info ) {
                    ExpressionResultBuilder expressionBuilder( it->type );
                        expressionBuilder << it->message;
                    AssertionInfo info( it->macroName, it->lineInfo, "", ResultDisposition::Normal );
                    AssertionResult result = expressionBuilder.buildResult( info );
                    m_legacyReporter->Result( result );
                }
            }
        }
        m_legacyReporter->Result( assertionStats.assertionResult );
        return true;
    }
    INTERNAL_CATCH_INLINE void LegacyReporterAdapter::sectionEnded( SectionStats const& sectionStats ) {
        if( sectionStats.missingAssertions )
            m_legacyReporter->NoAssertionsInSection( sectionStats.sectionInfo.name );
        m_legacyReporter->EndSection( sectionStats.sectionInfo.name, sectionStats.assertions );
    }
    INTERNAL_CATCH_INLINE void LegacyReporterAdapter::testCaseEnded( TestCaseStats const& testCaseStats ) {
        m_legacyReporter->EndTestCase
            (   testCaseStats.testInfo,
                testCaseStats.totals,
                testCaseStats.stdOut,
                testCaseStats.stdErr );
    }
    INTERNAL_CATCH_INLINE void LegacyReporterAdapter::testGroupEnded( TestGroupStats const& testGroupStats ) {
        if( testGroupStats.aborting )
            m_legacyReporter->Aborted();
        m_legacyReporter->EndGroup( testGroupStats.groupInfo.name, testGroupStats.totals );
    }
    INTERNAL_CATCH_INLINE void LegacyReporterAdapter::testRunEnded( TestRunStats const& testRunStats ) {
        m_legacyReporter->EndTesting( testRunStats.totals );
    }
}

#endif // TWOBLUECUBES_CATCH_LEGACY_REPORTER_ADAPTER_H_INCLUDED
