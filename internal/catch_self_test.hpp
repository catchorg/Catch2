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
    class SelfTestReporter : public IReporter
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        SelfTestReporter
        ()
        :   m_succeeded( 0 ),
            m_failed( 0 )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////////
        static std::string getDescription
        ()
        {
            return "Captures results for self test purposes";
        }
        
        ///////////////////////////////////////////////////////////////////////////
        size_t getSucceeded
        ()
        const
        {
            return m_succeeded;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        size_t getFailed
        ()
        const
        {
            return m_failed;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void reset()
        {
            m_succeeded = 0;
            m_failed = 0;
        }
        
    private: // IReporter
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTesting
        ()
        {}
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTesting
        (
            std::size_t succeeded, 
            std::size_t failed
        )
        {
            m_succeeded = succeeded;
            m_failed = failed;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        // Deliberately unimplemented:
        virtual void StartGroup( const std::string& ){}
        virtual void EndGroup( const std::string&, std::size_t, std::size_t ){}
        virtual void StartTestCase( const TestCaseInfo& ){}
        virtual void StartSection( const std::string&, const std::string ){}
        virtual void EndSection( const std::string&, std::size_t, std::size_t ){}
        virtual void Result( const ResultInfo& ){}
        virtual void EndTestCase( const TestCaseInfo&, std::size_t, std::size_t, const std::string&, const std::string& ){}
        
    private:
        size_t m_succeeded;
        size_t m_failed;
    };
    
    
    class SelfTestConfig : public Config
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        SelfTestConfig
        ()
        : m_reporter( new SelfTestReporter() )
        {
            // reporter will be deleted by the base config class
            setReporter( m_reporter );
            setStreamBuf( m_oss.rdbuf() );    
        }

        ///////////////////////////////////////////////////////////////////////////
        SelfTestReporter& getReporter
        ()
        {
            return *m_reporter;
        }
        
    private:
        std::ostringstream m_oss;
        SelfTestReporter* m_reporter;
        
    };
    
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
            std::size_t result;
            Runner runner( m_config );
            m_config.getReporter().reset();
            result = runner.runMatching( rawTestSpec );
            return result;
        }

        ///////////////////////////////////////////////////////////////////////////
        SelfTestReporter& getReporter
        ()
        {
            return m_config.getReporter();
        }
        
    private:
        SelfTestConfig m_config;
    };
}

#endif // TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
