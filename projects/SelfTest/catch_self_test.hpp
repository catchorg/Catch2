/*
 *  catch_self_test.hpp
 *  Catch
 *
 *  Created by Phil on 14/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED

#include "catch.hpp"

namespace Catch
{

    class EmbeddedRunner
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        EmbeddedRunner
        ()
        {
        }
        
        ///////////////////////////////////////////////////////////////////////////
        std::size_t runMatching
        (
            const std::string& rawTestSpec
        )
        {
            std::ostringstream oss;
            Config config;
            config.setStreamBuf( oss.rdbuf() );
            config.setReporter( "basic" );

            std::size_t result;
            
            // Scoped because Runner doesn't report EndTesting until its destructor
            {
                Runner runner( config );
                result = runner.runMatching( rawTestSpec );
                m_successes = runner.getSuccessCount();
                m_failures = runner.getFailureCount();
            }
            m_output = oss.str();
            return result;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        std::string getOutput
        ()
        {
            return m_output;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        std::size_t getSuccessCount
        ()
        const
        {
            return m_successes;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        std:: size_t getFailureCount
        ()
        const
        {
            return m_failures;
        }

    private:
        std::size_t m_successes;
        std::size_t m_failures;
        std::string m_output;
    };

    class MetaTestRunner
    {
    public:
        struct Expected
        {
            enum Result
            {
                ToSucceed,
                ToFail
            };
        };
        
        ///////////////////////////////////////////////////////////////////////////
        static void runMatching
        (
            const std::string& testSpec, 
            Expected::Result expectedResult
        )
        {
            forEach(    Hub::getTestCaseRegistry().getMatchingTestCases( testSpec ), 
                        MetaTestRunner( expectedResult ) );
        }
        
        ///////////////////////////////////////////////////////////////////////////
        MetaTestRunner
        (
            Expected::Result expectedResult
        )
        : m_expectedResult( expectedResult )
        {        
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void operator()
        (
            const TestCaseInfo& testCase
        )
        {
            EmbeddedRunner runner;
            runner.runMatching( testCase.getName() );
            switch( m_expectedResult )
            {
                case Expected::ToSucceed:
                    if( runner.getFailureCount() > 0 )
                    {
                        INFO( runner.getOutput() );
                        FAIL( "Expected test case '" 
                             << testCase.getName() 
                             << "' to succeed but there was/ were " 
                             << runner.getFailureCount() << " failure(s)" );
                    }
                    break;
                case Expected::ToFail:
                    if( runner.getSuccessCount() > 0 )
                    {
                        INFO( runner.getOutput() );
                        FAIL( "Expected test case '" 
                             << testCase.getName() 
                             << "' to fail but there was/ were " 
                             << runner.getSuccessCount() << " success(es)" );
                    }
                    break;
            }        
        };
        
    private:
        Expected::Result m_expectedResult;
    };
    
}

#endif // TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
