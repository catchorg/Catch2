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
    class iTchRunnerReporter : public SharedImpl<IReporter>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        iTchRunnerReporter
        (
            id<iTchRunnerDelegate> delegate
        )
        :   m_delegate( delegate )
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual bool shouldRedirectStdout
        ()
        const
        {
            return true;
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
            return m_totals.assertions.passed;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        size_t getFailed
        ()
        const
        {
            return m_totals.assertions.failed;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void reset()
        {
            m_totals = Totals();
        }
        
    private: // IReporter
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTesting
        ()
        {}
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTesting
        (
            const Totals& totals
        )
        {
            m_totals = totals;
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
        virtual void EndGroup( const std::string&, const Totals& ){}
        virtual void StartTestCase( const TestCaseInfo& ){}
        virtual void StartSection( const std::string& sectionName, const std::string& description ) {}
        virtual void EndSection( const std::string&, const Counts& ){}
        virtual void EndTestCase( const TestCaseInfo&, const Totals&, const std::string&, const std::string& ){}
        virtual void Aborted() {}
        
    private:
        Totals m_totals;
        
        id<iTchRunnerDelegate> m_delegate;
    };
}
