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

class MultipleReporters : public SharedImpl<IStreamingReporter> {
    typedef std::vector<Ptr<IStreamingReporter> > Reporters;
    Reporters m_reporters;

public:
    void add( Ptr<IStreamingReporter> const& reporter ) {
        m_reporters.push_back( reporter );
    }

public: // IStreamingReporter

    virtual ReporterPreferences getPreferences() const override {
        return m_reporters[0]->getPreferences();
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

    virtual MultipleReporters* tryAsMulti() override {
        return this;
    }

};

Ptr<IStreamingReporter> addReporter( Ptr<IStreamingReporter> const& existingReporter, Ptr<IStreamingReporter> const& additionalReporter ) {
    Ptr<IStreamingReporter> resultingReporter;

    if( existingReporter ) {
        MultipleReporters* multi = existingReporter->tryAsMulti();
        if( !multi ) {
            multi = new MultipleReporters;
            resultingReporter = Ptr<IStreamingReporter>( multi );
            if( existingReporter )
                multi->add( existingReporter );
        }
        else
            resultingReporter = existingReporter;
        multi->add( additionalReporter );
    }
    else
        resultingReporter = additionalReporter;

    return resultingReporter;
}


} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_MULTI_HPP_INCLUDED
