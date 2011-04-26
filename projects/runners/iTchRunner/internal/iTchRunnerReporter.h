/*
 *  iTchRunnerReporter.h
 *  iTchRunner
 *
 *  Created by Phil on 07/02/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 */
#include "catch.hpp"

@protocol iTchRunnerDelegate

-(void) testWasRun: (const Catch::ResultInfo*) result;    

@end

namespace Catch
{
    class iTchRunnerReporter : public IReporter
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        iTchRunnerReporter
        (
            id<iTchRunnerDelegate> delegate
        )
        :   m_succeeded( 0 ),
            m_failed( 0 ),
            m_delegate( delegate )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////////
        static std::string getDescription
        ()
        {
            return "Captures results for iOS runner";
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
        virtual void Result
        (
            const ResultInfo& result
        )
        {
            [m_delegate testWasRun: &result];
        }
        
        ///////////////////////////////////////////////////////////////////////////
        // Deliberately unimplemented:
        virtual void StartGroup( const std::string& ){}
        virtual void EndGroup( const std::string&, std::size_t, std::size_t ){}
        virtual void StartTestCase( const TestCaseInfo& ){}
        virtual void StartSection( const std::string&, const std::string ){}
        virtual void EndSection( const std::string&, std::size_t, std::size_t ){}
        virtual void EndTestCase( const TestCaseInfo&, std::size_t, std::size_t, const std::string&, const std::string& ){}
        
    private:
        size_t m_succeeded;
        size_t m_failed;
        
        id<iTchRunnerDelegate> m_delegate;
    };
}
