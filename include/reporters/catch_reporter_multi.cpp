/*
 *  Created by Phil on 5/08/2015.
 *  Copyright 2015 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

 #include "catch_reporter_multi.h"

namespace Catch {

    void MultipleReporters::add( IStreamingReporterPtr&& reporter ) {
        m_reporters.push_back( std::move( reporter ) );
    }

    ReporterPreferences MultipleReporters::getPreferences() const {
        return m_reporters[0]->getPreferences();
    }

    std::set<Verbosity> MultipleReporters::getSupportedVerbosities() {
        return std::set<Verbosity>{ };
    }


    void MultipleReporters::noMatchingTestCases( std::string const& spec ) {
        for( auto const& reporter : m_reporters )
            reporter->noMatchingTestCases( spec );
    }

    void MultipleReporters::benchmarkStarting( BenchmarkInfo const& benchmarkInfo ) {
        for( auto const& reporter : m_reporters )
            reporter->benchmarkStarting( benchmarkInfo );
    }
    void MultipleReporters::benchmarkEnded( BenchmarkStats const& benchmarkStats ) {
        for( auto const& reporter : m_reporters )
            reporter->benchmarkEnded( benchmarkStats );
    }

    void MultipleReporters::testRunStarting( TestRunInfo const& testRunInfo ) {
        for( auto const& reporter : m_reporters )
            reporter->testRunStarting( testRunInfo );
    }

    void MultipleReporters::testGroupStarting( GroupInfo const& groupInfo ) {
        for( auto const& reporter : m_reporters )
            reporter->testGroupStarting( groupInfo );
    }


    void MultipleReporters::testCaseStarting( TestCaseInfo const& testInfo ) {
        for( auto const& reporter : m_reporters )
            reporter->testCaseStarting( testInfo );
    }

    void MultipleReporters::sectionStarting( SectionInfo const& sectionInfo ) {
        for( auto const& reporter : m_reporters )
            reporter->sectionStarting( sectionInfo );
    }

    void MultipleReporters::assertionStarting( AssertionInfo const& assertionInfo ) {
        for( auto const& reporter : m_reporters )
            reporter->assertionStarting( assertionInfo );
    }

    // The return value indicates if the messages buffer should be cleared:
    bool MultipleReporters::assertionEnded( AssertionStats const& assertionStats ) {
        bool clearBuffer = false;
        for( auto const& reporter : m_reporters )
            clearBuffer |= reporter->assertionEnded( assertionStats );
        return clearBuffer;
    }

    void MultipleReporters::sectionEnded( SectionStats const& sectionStats ) {
        for( auto const& reporter : m_reporters )
            reporter->sectionEnded( sectionStats );
    }

    void MultipleReporters::testCaseEnded( TestCaseStats const& testCaseStats ) {
        for( auto const& reporter : m_reporters )
            reporter->testCaseEnded( testCaseStats );
    }

    void MultipleReporters::testGroupEnded( TestGroupStats const& testGroupStats ) {
        for( auto const& reporter : m_reporters )
            reporter->testGroupEnded( testGroupStats );
    }

    void MultipleReporters::testRunEnded( TestRunStats const& testRunStats ) {
        for( auto const& reporter : m_reporters )
            reporter->testRunEnded( testRunStats );
    }


    void MultipleReporters::skipTest( TestCaseInfo const& testInfo ) {
        for( auto const& reporter : m_reporters )
            reporter->skipTest( testInfo );
    }

    bool MultipleReporters::isMulti() const {
        return true;
    }

} // end namespace Catch
