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

#include "catch_reporter_registry.hpp"
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
    
    class Runner : public IResultListener
    {
    public:
        explicit Runner( ITestReporter* reporter )
        :   m_successes( 0 ),
            m_failures( 0 ),
            m_reporter( reporter )
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
            
            try
            {
                testInfo.invoke();
            }
            catch( TestFailureException& )
            {
                // This just means the test was aborted due to failure
            }
            catch( std::exception& ex )
            {
                ResultsCapture::acceptMessage( ex.what() );
                ResultsCapture::acceptResult( ResultWas::ThrewException );
            }
            catch(...)
            {
                ResultsCapture::acceptMessage( "unknown exception" );
                ResultsCapture::acceptResult( ResultWas::ThrewException );
            }

            m_reporter->EndTestCase( testInfo );
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
        
        virtual void testEnded( const ResultInfo& result )
        { 
            if( result.ok() )
                m_successes++;
            else
                m_failures++;
            
            m_reporter->Result( result );
        }

        virtual bool sectionStarted( const std::string& name, const std::string& description, std::size_t& successes, std::size_t& failures )
        {
            m_reporter->StartSection( name, description );
            successes = m_successes;
            failures = m_failures;
            return true;
        }
        
        virtual void sectionEnded( const std::string& name, std::size_t prevSuccesses, std::size_t prevFailures )
        {
            m_reporter->EndSection( name, m_successes - prevSuccesses, m_failures - prevFailures );
        }
        
    private:
        std::size_t m_successes;
        std::size_t m_failures;
        ITestReporter* m_reporter;
    };
}

#endif // TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED