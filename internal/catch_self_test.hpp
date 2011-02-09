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

#include "../catch.hpp"
#include "catch_runner_impl.hpp"

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
}

#endif // TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
