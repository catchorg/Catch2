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
#include "internal/catch_interfaces_reporter.h"
#include "internal/catch_reporter_registrars.hpp"

namespace Catch
{
    class BasicReporter : public IReporter
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        BasicReporter( const IReporterConfig& config )
        :   m_config( config )
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        static std::string getDescription()
        {
            return "Reports test results as lines of text";
        }
        
    private:
        
        ///////////////////////////////////////////////////////////////////////////
        void ReportCounts( std::size_t succeeded, std::size_t failed )
        {
            if( failed + succeeded == 0 )
                m_config.stream() << "No tests ran";
            else if( failed == 0 )
                m_config.stream() << "All " << succeeded << " test(s) succeeded";
            else if( succeeded == 0 )
                m_config.stream() << "All " << failed << " test(s) failed";
            else
                m_config.stream() << succeeded << " test(s) passed but " << failed << " test(s) failed";
        }
        
    private: // IReporter

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTesting()
        {
            m_config.stream() << "[Started testing]" << std::endl;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTesting( std::size_t succeeded, std::size_t failed )
        {
            m_config.stream() << "[Testing completed. ";
            ReportCounts( succeeded, failed );
            m_config.stream() << "]" << std::endl;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartGroup( const std::string& groupName )
        {
            if( !groupName.empty() )
                m_config.stream() << "[Started group: '" << groupName << "']" << std::endl;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndGroup( const std::string& groupName, std::size_t succeeded, std::size_t failed )
        {
            if( !groupName.empty() )
            {
                m_config.stream() << "[End of group: '" << groupName << "'. ";
                ReportCounts( succeeded, failed );
                m_config.stream() << "]\n" << std::endl;
            }
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTestCase( const TestCaseInfo& testInfo )
        {
            m_config.stream() << std::endl << "[Running: " << testInfo.getName() << "]" << std::endl;
            m_firstSectionInTestCase = true;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartSection( const std::string& sectionName, const std::string /*description*/ )
        {
            if( m_firstSectionInTestCase )
            {
                m_config.stream() << "\n";
                m_firstSectionInTestCase = false;
            }
            m_config.stream() << "[Started section: '" << sectionName << "']" << std::endl;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndSection( const std::string& sectionName, std::size_t succeeded, std::size_t failed )
        {
            m_config.stream() << "[End of section: '" << sectionName << "'. ";
            ReportCounts( succeeded, failed );
            m_config.stream() << "]\n" << std::endl;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void Result( const ResultInfo& resultInfo )
        {
            if( !m_config.includeSuccessfulResults() && resultInfo.getResultType() == ResultWas::Ok )
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
                default:
                    break;
            }
            
            if( resultInfo.hasExpression() )
            {
                m_config.stream() << " for: " << resultInfo.getExpandedExpression();
            }
            m_config.stream() << std::endl;        
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTestCase( const TestCaseInfo& testInfo, const std::string& stdOut, const std::string& stdErr )
        {
            if( !stdOut.empty() )
                m_config.stream() << "[stdout: " << trim( stdOut ) << "]\n";

            if( !stdErr.empty() )
                m_config.stream() << "[stderr: " << trim( stdErr ) << "]\n";
            
            m_config.stream() << "[Finished: " << testInfo.getName() << "]" << std::endl;
        }    
        
    private:
        const IReporterConfig& m_config;
        bool m_firstSectionInTestCase;
    };

    CATCH_REGISTER_REPORTER( "basic", BasicReporter );
    
} // end namespace Catch
    
#endif // TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED
