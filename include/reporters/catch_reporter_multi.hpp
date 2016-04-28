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

    virtual ReporterPreferences getPreferences() const CATCH_OVERRIDE {
        return m_reporters[0]->getPreferences();
    }

    virtual void noMatchingTestCases( std::string const& spec ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->noMatchingTestCases( spec );
    }


    virtual void testRunStarting( TestRunInfo const& testRunInfo ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->testRunStarting( testRunInfo );
    }

    virtual void testGroupStarting( GroupInfo const& groupInfo ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->testGroupStarting( groupInfo );
    }


    virtual void testCaseStarting( TestCaseInfo const& testInfo ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->testCaseStarting( testInfo );
    }

    virtual void sectionStarting( SectionInfo const& sectionInfo ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->sectionStarting( sectionInfo );
    }


    virtual void assertionStarting( AssertionInfo const& assertionInfo ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->assertionStarting( assertionInfo );
    }


    // The return value indicates if the messages buffer should be cleared:
    virtual bool assertionEnded( AssertionStats const& assertionStats ) CATCH_OVERRIDE {
        bool clearBuffer = false;
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            clearBuffer |= (*it)->assertionEnded( assertionStats );
        return clearBuffer;
    }

    virtual void sectionEnded( SectionStats const& sectionStats ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->sectionEnded( sectionStats );
    }

    virtual void testCaseEnded( TestCaseStats const& testCaseStats ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->testCaseEnded( testCaseStats );
    }

    virtual void testGroupEnded( TestGroupStats const& testGroupStats ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->testGroupEnded( testGroupStats );
    }

    virtual void testRunEnded( TestRunStats const& testRunStats ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->testRunEnded( testRunStats );
    }


    virtual void skipTest( TestCaseInfo const& testInfo ) CATCH_OVERRIDE {
        for( Reporters::const_iterator it = m_reporters.begin(), itEnd = m_reporters.end();
                it != itEnd;
                ++it )
            (*it)->skipTest( testInfo );
    }
    
    virtual MultipleReporters* tryAsMulti() CATCH_OVERRIDE {
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
