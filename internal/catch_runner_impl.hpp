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

#include "catch_interfaces_reporter.h"
#include "catch_runnerconfig.hpp"
#include "catch_registry.hpp"
#include "catch_capture.hpp"

namespace Catch
{
    class TestSpec
    {
    public:
        TestSpec( const std::string& rawSpec )
        :   m_rawSpec( rawSpec ),
            m_isWildcarded( false )
        {
            if( m_rawSpec[m_rawSpec.size()-1] == '*' )
            {
                m_rawSpec = m_rawSpec.substr( 0, m_rawSpec.size()-1 );
                m_isWildcarded = true;
            }
        }
        
        bool matches( const std::string& testName ) const
        {
            if( !m_isWildcarded )
                return m_rawSpec == testName;
            else
                return testName.size() >= m_rawSpec.size() && testName.substr( 0, m_rawSpec.size() ) == m_rawSpec;            
        }
        
    private:
        std::string m_rawSpec;
        bool m_isWildcarded;
    };
    
    class StreamRedirect
    {
    public:
        StreamRedirect( std::ostream& stream, std::string& targetString )
        :   m_stream( stream ),
        m_prevBuf( stream.rdbuf() ),
        m_targetString( targetString )
        {            
            stream.rdbuf( m_oss.rdbuf() );
        }
        
        ~StreamRedirect()
        {
            m_targetString = m_oss.str();
            m_stream.rdbuf( m_prevBuf );
        }
        
    private:
        std::ostream& m_stream;
        std::streambuf* m_prevBuf;
        std::ostringstream m_oss;
        std::string& m_targetString;
    };
    
    class Runner : public IResultListener
    {
        Runner( const Runner& );
        void operator =( const Runner& );
        
    public:
        explicit Runner( const RunnerConfig& config )
        :   m_config( config ),
            m_successes( 0 ),
            m_failures( 0 ),
            m_reporter( m_config.getReporter() )
        {
            m_reporter->StartTesting();
        }
        
        ~Runner()
        {
            m_reporter->EndTesting( m_successes, m_failures );
        }
        
        void runAll()
        {
            std::vector<TestCaseInfo> allTests = TestRegistry::instance().getAllTests();
            for( std::size_t i=0; i < allTests.size(); ++i )
            {
                runTest( allTests[i] );
            }
        }
        
        std::size_t runMatching( const std::string& rawTestSpec )
        {
            TestSpec testSpec( rawTestSpec );
            
            std::vector<TestCaseInfo> allTests = TestRegistry::instance().getAllTests();
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
        
        void runTest( const TestCaseInfo& testInfo )
        {
            IResultListener* prevListener = ResultsCapture::setListener( this );
            m_reporter->StartTestCase( testInfo );
            
            std::string redirectedCout;
            std::string redirectedCerr;

            try
            {
                StreamRedirect coutRedir( std::cout, redirectedCout );
                StreamRedirect cerrRedir( std::cerr, redirectedCerr );
                testInfo.invoke();                
            }
            catch( TestFailureException& )
            {
                // This just means the test was aborted due to failure
            }
            catch( std::exception& ex )
            {
                acceptMessage( ex.what() );
                acceptResult( ResultWas::ThrewException );
            }
            catch(...)
            {
                acceptMessage( "unknown exception" );
                acceptResult( ResultWas::ThrewException );
            }
            m_info.clear();
            m_reporter->EndTestCase( testInfo, redirectedCout, redirectedCerr );
            ResultsCapture::setListener( prevListener );
        }
        
        std::size_t getSuccessCount() const
        {
            return m_successes;
        }
       std:: size_t getFailureCount() const
        {
            return m_failures;
        }

    private: // IResultListener

        virtual ResultAction::Value acceptResult( bool result )
        {
            return acceptResult( result ? ResultWas::Ok : ResultWas::ExpressionFailed );
        }

        virtual ResultAction::Value acceptResult( ResultWas::OfType result )
        {
            m_currentResult.setResultType( result );            
            testEnded( m_currentResult );

            bool ok = m_currentResult.ok();
            m_currentResult = MutableResultInfo();
            if( ok )
                return ResultAction::None;
            else if( shouldDebugBreak() )
                return ResultAction::DebugFailed;
            else
                return ResultAction::Failed;
            
        }

        virtual void acceptExpression( const MutableResultInfo& resultInfo )
        {
            m_currentResult = resultInfo;
        }

        virtual void acceptMessage( const std::string& msg )
        {
            m_currentResult.setMessage( msg );
        }
        
        ///
        
        virtual void testEnded( const ResultInfo& result )
        { 
            if( result.getResultType() == ResultWas::Ok )
            {
                m_successes++;
            }
            else if( !result.ok() )
            {
                m_failures++;

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

        virtual bool sectionStarted( const std::string& name, const std::string& description, std::size_t& successes, std::size_t& failures )
        {
            m_reporter->StartSection( name, description );
            successes = m_successes;
            failures = m_failures;
            
            // !TBD look up whether we should execute this section or not
            return true;
        }
        
        virtual void sectionEnded( const std::string& name, std::size_t prevSuccesses, std::size_t prevFailures )
        {
            m_reporter->EndSection( name, m_successes - prevSuccesses, m_failures - prevFailures );
        }

        virtual void pushScopedInfo( ScopedInfo* scopedInfo )
        {
            m_scopedInfos.push_back( scopedInfo );
        }
        virtual void popScopedInfo( ScopedInfo* scopedInfo )
        {
            if( m_scopedInfos.back() == scopedInfo )
                m_scopedInfos.pop_back();
        }
        virtual bool shouldDebugBreak() const
        {
            return m_config.shouldDebugBreak();
        }
        
    private:
        MutableResultInfo m_currentResult;

        const RunnerConfig& m_config;
        std::size_t m_successes;
        std::size_t m_failures;
        IReporter* m_reporter;
        std::vector<ScopedInfo*> m_scopedInfos;
        std::vector<ResultInfo> m_info;
    };
}

#endif // TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED
