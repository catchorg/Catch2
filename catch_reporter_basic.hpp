/*
 *  catch_reporter_basic.hpp
 *  Catch
 *
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED

#include "internal/catch_capture.hpp"
#include "internal/catch_reporter_registry.hpp"

namespace Catch
{
    class BasicReporter : public ITestReporter
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        BasicReporter( const ReporterConfig& config )
        :   m_config( config )
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        static std::string getDescription()
        {
            return "Reports test results as lines of text";
        }
        
    private: // ITestReporter
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTestCase( const TestCaseInfo& testInfo )
        {
            m_config.stream() << std::endl << "[Running: " << testInfo.getName() << "]" << std::endl;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void Result( const ResultInfo& resultInfo )
        {
            if( !m_config.includeSuccessfulResults() && resultInfo.ok() )
                return;
            
            if( !resultInfo.getFilename().empty() )
                m_config.stream() << resultInfo.getFilename() << "(" << resultInfo.getLine() << "): ";
            
            if( resultInfo.hasExpression() )
            {
                m_config.stream() << resultInfo.getExpression();
                if( resultInfo.ok() )
                    m_config.stream() << " succeeded";
                else
                    m_config.stream() << " failed";
            }
            switch( resultInfo.getResultType() )
            {
                case ResultWas::ThrewException:
                    if( resultInfo.hasExpression() )
                        m_config.stream() << " with unexpected";
                    else
                        m_config.stream() << "Unexpected";
                    m_config.stream() << " exception with message: '" << resultInfo.getMessage() << "'";
                    break;
                case ResultWas::Info:
                    m_config.stream() << "info: '" << resultInfo.getMessage() << "'";
                    break;
                case ResultWas::Warning:
                    m_config.stream() << "warning: '" << resultInfo.getMessage() << "'";
                    break;
                case ResultWas::ExplicitFailure:
                    m_config.stream() << "failed with message: '" << resultInfo.getMessage() << "'";
                    break;
            }
            
            if( resultInfo.hasExpression() )
            {
                m_config.stream() << " for: " << resultInfo.getExpandedExpression();
            }
            m_config.stream() << std::endl;        
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTestCase( const TestCaseInfo& testInfo )
        {
            m_config.stream() << "[Finished: " << testInfo.getName() << "]" << std::endl;
        }    
        
    private:
        const ReporterConfig& m_config;
    };
    
} // end namespace Catch
    
#endif // TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED