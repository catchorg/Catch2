 /*
 *  catch_runner.hpp
 *  Catch
 *
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED
#define TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED

#include "catch_interfaces_runner.h"
#include "catch_interfaces_reporter.h"
#include "catch_config.hpp"
#include "catch_test_registry.hpp"
#include "catch_test_case_info.hpp"
#include "catch_capture.hpp"
#include "catch_totals.hpp"

#include <set>
#include <string>

namespace Catch
{
    
    class StreamRedirect
    {
    public:
        ///////////////////////////////////////////////////////////////////////        
        StreamRedirect
        (
            std::ostream& stream, 
            std::string& targetString
        )
        :   m_stream( stream ),
            m_prevBuf( stream.rdbuf() ),
            m_targetString( targetString )
        {            
            stream.rdbuf( m_oss.rdbuf() );
        }
        
        ///////////////////////////////////////////////////////////////////////        
        ~StreamRedirect
        ()
        {
            m_targetString += m_oss.str();
            m_stream.rdbuf( m_prevBuf );
        }
        
    private:
        std::ostream& m_stream;
        std::streambuf* m_prevBuf;
        std::ostringstream m_oss;
        std::string& m_targetString;
    };
    
    
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////    
    class SectionInfo
    {
    public:
        enum Status
        {
            Root,
            Unknown,
            NonLeaf,
            TestedLeaf
        };
        
        ///////////////////////////////////////////////////////////////////////        
        SectionInfo
        (
            SectionInfo* parent
        )
        :   m_status( Unknown ),
            m_parent( parent )
        {
        }

        ///////////////////////////////////////////////////////////////////////        
        SectionInfo
        ()
        :   m_status( Root ),
            m_parent( NULL )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////
        ~SectionInfo
        ()
        {
            deleteAllValues( m_subSections );
        }
        
        ///////////////////////////////////////////////////////////////////////
        bool shouldRun
        ()
        const
        {
            return m_status != TestedLeaf;
        }
        
        ///////////////////////////////////////////////////////////////////////
        bool ran
        ()
        {
            if( m_status != NonLeaf )
            {
                m_status = TestedLeaf;
                return true;
            }
            return false;
        }
        
        ///////////////////////////////////////////////////////////////////////
        SectionInfo* findSubSection
        (
            const std::string& name
        )
        {
            std::map<std::string, SectionInfo*>::const_iterator it = m_subSections.find( name );
            return it != m_subSections.end()
                ? it->second
                : NULL;
        }
        
        ///////////////////////////////////////////////////////////////////////
        SectionInfo* addSubSection
        (
            const std::string& name
        )
        {
            SectionInfo* subSection = new SectionInfo( this );
            m_subSections.insert( std::make_pair( name, subSection ) );
            m_status = NonLeaf;
            return subSection;
        }
        
        ///////////////////////////////////////////////////////////////////////
        SectionInfo* getParent
        ()
        {
            return m_parent;
        }
        
        ///////////////////////////////////////////////////////////////////////        
        bool hasUntestedSections
        ()
        const
        {
            if( m_status == Unknown )
                return true;
            
            std::map<std::string, SectionInfo*>::const_iterator it = m_subSections.begin();
            std::map<std::string, SectionInfo*>::const_iterator itEnd = m_subSections.end();
            for(; it != itEnd; ++it )
            {
                if( it->second->hasUntestedSections() )
                    return true;
            }
            return false;
        }
    
    private:
        Status m_status;
        std::map<std::string, SectionInfo*> m_subSections;
        SectionInfo* m_parent;
    };
    
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    class RunningTest
    {
        enum RunStatus
        {
            NothingRun,
            EncounteredASection,
            RanAtLeastOneSection,
            RanToCompletionWithSections,
            RanToCompletionWithNoSections
        };
        
    public:
        ///////////////////////////////////////////////////////////////////////        
        explicit RunningTest
        (
            const TestCaseInfo* info = NULL 
        )
        :   m_info( info ),
            m_runStatus( RanAtLeastOneSection ),
            m_currentSection( &m_rootSection ),
            m_changed( false )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////        
        bool wasSectionSeen
        ()
        const
        {
            return  m_runStatus == RanAtLeastOneSection || 
                    m_runStatus == RanToCompletionWithSections;
        }

        ///////////////////////////////////////////////////////////////////////        
        void reset
        ()
        {
            m_runStatus = NothingRun;
            m_changed = false;
        }

        ///////////////////////////////////////////////////////////////////////        
        void ranToCompletion
        ()
        {
            m_runStatus =   m_runStatus == RanAtLeastOneSection ||
                            m_runStatus == EncounteredASection
                ? RanToCompletionWithSections
                : RanToCompletionWithNoSections;
        }
        
        ///////////////////////////////////////////////////////////////////////        
        bool addSection
        (
            const std::string& name
        )
        {
            if( m_runStatus == NothingRun )
                m_runStatus = EncounteredASection;
            
            SectionInfo* thisSection = m_currentSection->findSubSection( name );
            if( !thisSection )
            {
                thisSection = m_currentSection->addSubSection( name );
                m_changed = true;
            }
            
            if( !wasSectionSeen() && thisSection->shouldRun() )
            {
                m_currentSection = thisSection;
                return true;
            }
            return false;
        }

        ///////////////////////////////////////////////////////////////////////        
        void endSection
        (
            const std::string&
        )
        {
            if( m_currentSection->ran() )
            {
                m_runStatus = RanAtLeastOneSection;
                m_changed = true;
            }
            m_currentSection = m_currentSection->getParent();
        }

        ///////////////////////////////////////////////////////////////////////        
        const TestCaseInfo& getTestCaseInfo
        ()
        const
        {
            return *m_info;
        }
        
        ///////////////////////////////////////////////////////////////////////        
        bool hasUntestedSections
        ()
        const
        {
            return  m_runStatus == RanAtLeastOneSection ||
                    ( m_rootSection.hasUntestedSections() && m_changed );
        }
        
    private:
        const TestCaseInfo* m_info;
        RunStatus m_runStatus;
        SectionInfo m_rootSection;
        SectionInfo* m_currentSection;
        bool m_changed;
    };
    
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////    
    class Runner : public IResultCapture, public IRunner
    {
        Runner( const Runner& );
        void operator =( const Runner& );
        
    public:

        ///////////////////////////////////////////////////////////////////////////
        explicit Runner
        (
            const Config& config 
        )
        :   m_runningTest( NULL ),
            m_config( config ),
            m_reporter( m_config.getReporter() ),
            m_prevRunner( &Hub::getRunner() ),
            m_prevResultCapture( &Hub::getResultCapture() )
        {
            Hub::setRunner( this );
            Hub::setResultCapture( this );
            m_reporter->StartTesting();
        }
        
        ///////////////////////////////////////////////////////////////////////////
        ~Runner
        ()
        {
            m_reporter->EndTesting( m_totals );
            Hub::setRunner( m_prevRunner );
            Hub::setResultCapture( m_prevResultCapture );
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void runAll
        (
            bool runHiddenTests = false
        )
        {
            std::vector<TestCaseInfo> allTests = Hub::getTestCaseRegistry().getAllTests();
            for( std::size_t i=0; i < allTests.size(); ++i )
            {
                if( runHiddenTests || !allTests[i].isHidden() )
                   runTest( allTests[i] );
            }
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual std::size_t runMatching
        (
            const std::string& rawTestSpec
        )
        {
            TestSpec testSpec( rawTestSpec );
            
            std::vector<TestCaseInfo> allTests = Hub::getTestCaseRegistry().getAllTests();
            std::size_t testsRun = 0;
            for( std::size_t i=0; i < allTests.size(); ++i )
            {
                if( testSpec.matches( allTests[i].getName() ) )
                {
                    runTest( allTests[i] );
                    testsRun++;
                }
            }
            return testsRun;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void runTest
        (
            const TestCaseInfo& testInfo
        )
        {
            Totals prevTotals = m_totals;

            std::string redirectedCout;
            std::string redirectedCerr;
            
            m_reporter->StartTestCase( testInfo );
            
            m_runningTest = new RunningTest( &testInfo );

            do
            {
                do
                {
                    m_currentResult.setFileAndLine( m_runningTest->getTestCaseInfo().getFilename(), 
                                                    m_runningTest->getTestCaseInfo().getLine() );
                    runCurrentTest( redirectedCout, redirectedCerr );
                }
                while( m_runningTest->hasUntestedSections() );
            }
            while( Hub::advanceGeneratorsForCurrentTest() );

            delete m_runningTest;
            m_runningTest = NULL;

            if( m_totals.assertions.failed > prevTotals.assertions.failed )
                ++m_totals.testCases.failed;
            else
                ++m_totals.testCases.passed;
            
            m_reporter->EndTestCase( testInfo, m_totals - prevTotals, redirectedCout, redirectedCerr );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual Totals getTotals
        ()
        const
        {
            return m_totals;
        }
        
    private: // IResultCapture

        ///////////////////////////////////////////////////////////////////////////
        virtual ResultAction::Value acceptResult
        (
            bool result
        )
        {
            return acceptResult( result ? ResultWas::Ok : ResultWas::ExpressionFailed );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual ResultAction::Value acceptResult
        (
            ResultWas::OfType result
        )
        {
            m_currentResult.setResultType( result );            
            return actOnCurrentResult();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual ResultAction::Value acceptExpression
        (
            const MutableResultInfo& resultInfo
        )
        {
            m_currentResult = resultInfo;
            return actOnCurrentResult();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void acceptMessage
        (
            const std::string& msg
        )
        {
            m_currentResult.setMessage( msg );
        }
                
        ///////////////////////////////////////////////////////////////////////////
        virtual void testEnded
        (
            const ResultInfo& result        
        )
        { 
            if( result.getResultType() == ResultWas::Ok )
            {
                m_totals.assertions.passed++;
            }
            else if( !result.ok() )
            {
                m_totals.assertions.failed++;

                std::vector<ResultInfo>::const_iterator it = m_info.begin();
                std::vector<ResultInfo>::const_iterator itEnd = m_info.end();
                for(; it != itEnd; ++it )
                    m_reporter->Result( *it );
                m_info.clear();
            }
            
            if( result.getResultType() == ResultWas::Info )
                m_info.push_back( result );
            else
                m_reporter->Result( result );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual bool sectionStarted
        (
            const std::string& name, 
            const std::string& description,
            const std::string& filename,
            std::size_t line,
            Counts& assertions
        )
        {
            std::ostringstream oss;
            oss << name << "@" << SourceLineInfo( filename, line );

            if( !m_runningTest->addSection( oss.str() ) )
                return false;

            m_currentResult.setFileAndLine( filename, line );
            m_reporter->StartSection( name, description );
            assertions = m_totals.assertions;
            
            return true;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void sectionEnded
        (
            const std::string& name,
            const Counts& prevAssertions
        )
        {
            m_runningTest->endSection( name );
            m_reporter->EndSection( name, m_totals.assertions - prevAssertions );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void pushScopedInfo
        (
            ScopedInfo* scopedInfo 
        )
        {
            m_scopedInfos.push_back( scopedInfo );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void popScopedInfo
        (
            ScopedInfo* scopedInfo
        )
        {
            if( m_scopedInfos.back() == scopedInfo )
                m_scopedInfos.pop_back();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual bool shouldDebugBreak
        () 
        const
        {
            return m_config.shouldDebugBreak();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual std::string getCurrentTestName
        () 
        const
        {
            return m_runningTest
                ? m_runningTest->getTestCaseInfo().getName()
                : "";
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual const ResultInfo* getLastResult
        ()
        const
        {
            return &m_lastResult;            
        }
        
    private:
        
        ///////////////////////////////////////////////////////////////////////////
        ResultAction::Value actOnCurrentResult
        ()
        {
            testEnded( m_currentResult );
            m_lastResult = m_currentResult;
            
            m_currentResult = MutableResultInfo();
            if( m_lastResult.ok() )
                return ResultAction::None;
            else if( shouldDebugBreak() )
                return ResultAction::DebugFailed;
            else
                return ResultAction::Failed;
        }

        ///////////////////////////////////////////////////////////////////////////
        void runCurrentTest
        (
            std::string& redirectedCout,
            std::string& redirectedCerr
        )
        {            
            try
            {
                m_runningTest->reset();
                if( m_reporter->shouldRedirectStdout() )
                {
                    StreamRedirect coutRedir( std::cout, redirectedCout );
                    StreamRedirect cerrRedir( std::cerr, redirectedCerr );
                    m_runningTest->getTestCaseInfo().invoke();
                }
                else
                {
                    m_runningTest->getTestCaseInfo().invoke();
                }
                m_runningTest->ranToCompletion();
            }
            catch( TestFailureException& )
            {
                // This just means the test was aborted due to failure
            }
            catch(...)
            {
                acceptMessage( Catch::Hub::getExceptionTranslatorRegistry().translateActiveException() );
                acceptResult( ResultWas::ThrewException );
            }
            m_info.clear();
        }
        
    private:
        RunningTest* m_runningTest;
        MutableResultInfo m_currentResult;
        ResultInfo m_lastResult;

        const Config& m_config;
        Totals m_totals;
        IReporter* m_reporter;
        std::vector<ScopedInfo*> m_scopedInfos;
        std::vector<ResultInfo> m_info;
        IRunner* m_prevRunner;
        IResultCapture* m_prevResultCapture;
    };
}

#endif // TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED
