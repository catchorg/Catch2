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
    Ptr<IStreamingReporter> m_reporter;
    Ptr<MultipleReporters> m_next;

public:
    MultipleReporters( Ptr<IStreamingReporter> const& reporter )
        : m_reporter( reporter )
        , m_next( CATCH_NULL )
    {
    }

    MultipleReporters( Ptr<IStreamingReporter> const& first, Ptr<IStreamingReporter> const& second )
        : m_reporter( first )
        , m_next( new MultipleReporters( second ) )
    {
    }

    void add( Ptr<IStreamingReporter> const& reporter ) {
        if( m_next )
            m_next->add( reporter );
        else
            m_next = new MultipleReporters( reporter );
    }

public: // IStreamingReporter

    virtual ReporterPreferences getPreferences() const CATCH_OVERRIDE {
        return m_reporter->getPreferences();
    }

    virtual void noMatchingTestCases( std::string const& spec ) CATCH_OVERRIDE {
        m_reporter->noMatchingTestCases( spec );
        if( m_next )
            m_next->noMatchingTestCases( spec );
    }


    virtual void testRunStarting( TestRunInfo const& testRunInfo ) CATCH_OVERRIDE {
        m_reporter->testRunStarting( testRunInfo );
        if( m_next )
            m_next->testRunStarting( testRunInfo );
    }

    virtual void testGroupStarting( GroupInfo const& groupInfo ) CATCH_OVERRIDE {
        m_reporter->testGroupStarting( groupInfo );
        if( m_next )
            m_next->testGroupStarting( groupInfo );
    }


    virtual void testCaseStarting( TestCaseInfo const& testInfo ) CATCH_OVERRIDE {
        m_reporter->testCaseStarting( testInfo );
        if( m_next )
            m_next->testCaseStarting( testInfo );
    }

    virtual void sectionStarting( SectionInfo const& sectionInfo ) CATCH_OVERRIDE {
        m_reporter->sectionStarting( sectionInfo );
        if( m_next )
            m_next->sectionStarting( sectionInfo );
    }


    virtual void assertionStarting( AssertionInfo const& assertionInfo ) CATCH_OVERRIDE {
        m_reporter->assertionStarting( assertionInfo );
        if( m_next )
            m_next->assertionStarting( assertionInfo );
    }


    // The return value indicates if the messages buffer should be cleared:
    virtual bool assertionEnded( AssertionStats const& assertionStats ) CATCH_OVERRIDE {
        bool clearBuffer = m_reporter->assertionEnded( assertionStats );
        // Be careful with how the two calls' return values are anded.
        // We don't want short-circuiting to eliminate one of the calls.
        if( m_next && m_next->assertionEnded( assertionStats ) )
            return true;
        return clearBuffer;
    }

    virtual void sectionEnded( SectionStats const& sectionStats ) CATCH_OVERRIDE {
        m_reporter->sectionEnded( sectionStats );
        if( m_next )
            m_next->sectionEnded( sectionStats );
    }

    virtual void testCaseEnded( TestCaseStats const& testCaseStats ) CATCH_OVERRIDE {
        m_reporter->testCaseEnded( testCaseStats );
        if( m_next )
            m_next->testCaseEnded( testCaseStats );
    }

    virtual void testGroupEnded( TestGroupStats const& testGroupStats ) CATCH_OVERRIDE {
        m_reporter->testGroupEnded( testGroupStats );
        if( m_next )
            m_next->testGroupEnded( testGroupStats );
    }

    virtual void testRunEnded( TestRunStats const& testRunStats ) CATCH_OVERRIDE {
        m_reporter->testRunEnded( testRunStats );
        if( m_next )
            m_next->testRunEnded( testRunStats );
    }


    virtual void skipTest( TestCaseInfo const& testInfo ) CATCH_OVERRIDE {
        m_reporter->skipTest( testInfo );
        if( m_next )
            m_next->skipTest( testInfo );
    }
};

Ptr<IStreamingReporter> addReporter( Ptr<IStreamingReporter> const& existingReporter, Ptr<IStreamingReporter> const& additionalReporter ) {
    if( !existingReporter )
        return additionalReporter;
    return Ptr<IStreamingReporter>( new MultipleReporters(existingReporter, additionalReporter) );
}


} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_MULTI_HPP_INCLUDED
