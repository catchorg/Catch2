/*
 *  Created by Phil on 5/08/2015.
 *  Copyright 2015 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_MULTI_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_MULTI_HPP_INCLUDED

#include "../internal/catch_interfaces_reporter.h"

namespace Catch {

class MultipleReporters : public IStreamingReporter {
    typedef std::vector<IStreamingReporterPtr> Reporters;
    Reporters m_reporters;
    std::set<Verbosity> m_verbosities;

public:
    void add( IStreamingReporterPtr&& reporter ) {
        if( m_reporters.empty() ) {
            m_verbosities = reporter->getSupportedVerbosities();
        }
        else {
            for( auto it = m_verbosities.cbegin(); it != m_verbosities.cend(); ) {
                if( reporter->getSupportedVerbosities().count( *it ) == 0 ) {
                    it = m_verbosities.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        m_reporters.push_back( std::move( reporter ) );
    }

public: // IStreamingReporter

    virtual ReporterPreferences getPreferences() const override {
        return m_reporters[0]->getPreferences();
    }

    virtual std::set<Verbosity> const& getSupportedVerbosities() const override {
        return m_verbosities;
    }

    virtual void noMatchingTestCases( std::string const& spec ) override {
        for( auto const& reporter : m_reporters )
            reporter->noMatchingTestCases( spec );
    }


    virtual void testRunStarting( TestRunInfo const& testRunInfo ) override {
        for( auto const& reporter : m_reporters )
            reporter->testRunStarting( testRunInfo );
    }

    virtual void testGroupStarting( GroupInfo const& groupInfo ) override {
        for( auto const& reporter : m_reporters )
            reporter->testGroupStarting( groupInfo );
    }


    virtual void testCaseStarting( TestCaseInfo const& testInfo ) override {
        for( auto const& reporter : m_reporters )
            reporter->testCaseStarting( testInfo );
    }

    virtual void sectionStarting( SectionInfo const& sectionInfo ) override {
        for( auto const& reporter : m_reporters )
            reporter->sectionStarting( sectionInfo );
    }


    virtual void assertionStarting( AssertionInfo const& assertionInfo ) override {
        for( auto const& reporter : m_reporters )
            reporter->assertionStarting( assertionInfo );
    }


    // The return value indicates if the messages buffer should be cleared:
    virtual bool assertionEnded( AssertionStats const& assertionStats ) override {
        bool clearBuffer = false;
        for( auto const& reporter : m_reporters )
            clearBuffer |= reporter->assertionEnded( assertionStats );
        return clearBuffer;
    }

    virtual void sectionEnded( SectionStats const& sectionStats ) override {
        for( auto const& reporter : m_reporters )
            reporter->sectionEnded( sectionStats );
    }

    virtual void testCaseEnded( TestCaseStats const& testCaseStats ) override {
        for( auto const& reporter : m_reporters )
            reporter->testCaseEnded( testCaseStats );
    }

    virtual void testGroupEnded( TestGroupStats const& testGroupStats ) override {
        for( auto const& reporter : m_reporters )
            reporter->testGroupEnded( testGroupStats );
    }

    virtual void testRunEnded( TestRunStats const& testRunStats ) override {
        for( auto const& reporter : m_reporters )
            reporter->testRunEnded( testRunStats );
    }


    virtual void skipTest( TestCaseInfo const& testInfo ) override {
        for( auto const& reporter : m_reporters )
            reporter->skipTest( testInfo );
    }

    virtual bool isMulti() const override {
        return true;
    }

};

void addReporter( IStreamingReporterPtr& existingReporter, IStreamingReporterPtr&& additionalReporter ) {

    if( !existingReporter ) {
        existingReporter = std::move( additionalReporter );
        return;
    }

    MultipleReporters* multi = nullptr;

    if( existingReporter->isMulti() ) {
        multi = static_cast<MultipleReporters*>( existingReporter.get() );
    }
    else {
        auto newMulti = std::unique_ptr<MultipleReporters>( new MultipleReporters );
        newMulti->add( std::move( existingReporter ) );
        multi = newMulti.get();
        existingReporter = std::move( newMulti );
    }
    multi->add( std::move( additionalReporter ) );
}


} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_MULTI_HPP_INCLUDED
