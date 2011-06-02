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

#include "../internal/catch_capture.hpp"
#include "../internal/catch_interfaces_reporter.h"
#include "../internal/catch_reporter_registrars.hpp"

namespace Catch
{
    class BasicReporter : public IReporter
    {
        struct SpanInfo
        {
            SpanInfo() 
            :   emitted( false )
            {}
            
            SpanInfo( const std::string& spanName ) 
            :   name( spanName ),
                emitted( false )
            {}
            
            SpanInfo( const SpanInfo& other ) 
            :   name( other.name ),
                emitted( other.emitted )
            {}
            
            std::string name;
            bool emitted;
        };
        
    public:
        ///////////////////////////////////////////////////////////////////////////
        BasicReporter
        (
            const IReporterConfig& config
        )
        :   m_config( config )
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        static std::string getDescription
        ()
        {
            return "Reports test results as lines of text";
        }
        
    private:
        
        ///////////////////////////////////////////////////////////////////////////
        void ReportCounts
        (
            std::size_t succeeded, 
            std::size_t failed
        )
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
        virtual void StartTesting
        ()
        {
            m_testingSpan = SpanInfo();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTesting
        (
            std::size_t succeeded, 
            std::size_t failed
        )
        {
            // Output the overall test results even if "Started Testing" was not emitted
            m_config.stream() << "[Testing completed. ";
            ReportCounts( succeeded, failed );
            m_config.stream() << "]\n" << std::endl;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartGroup
        (
            const std::string& groupName
        )
        {
            m_groupSpan = groupName;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndGroup
        (
            const std::string& groupName, 
            std::size_t succeeded, 
            std::size_t failed 
        )
        {
            if( m_groupSpan.emitted && !groupName.empty() )
            {
                m_config.stream() << "[End of group: '" << groupName << "'. ";
                ReportCounts( succeeded, failed );
                m_config.stream() << "]\n" << std::endl;
                m_groupSpan = SpanInfo();
            }
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTestCase
        (
            const TestCaseInfo& testInfo
        )
        {
            m_testSpan = testInfo.getName();
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartSection
        (
            const std::string& sectionName, 
            const std::string /*description*/ 
        )
        {
            m_sectionSpans.push_back( SpanInfo( sectionName ) );
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndSection
        (
            const std::string& sectionName, 
            std::size_t succeeded, 
            std::size_t failed
        )
        {
            SpanInfo& sectionSpan = m_sectionSpans.back();
            if( sectionSpan.emitted && !sectionSpan.name.empty() )
            {
                m_config.stream() << "[End of section: '" << sectionName << "'. ";
                ReportCounts( succeeded, failed );
                m_config.stream() << "]\n" << std::endl;
            }
            m_sectionSpans.pop_back();
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void Result
        (
            const ResultInfo& resultInfo
        )
        {
            if( !m_config.includeSuccessfulResults() && resultInfo.getResultType() == ResultWas::Ok )
                return;
            
            StartSpansLazily();
            
            if( !resultInfo.getFilename().empty() )
#ifndef __GNUG__
                m_config.stream() << resultInfo.getFilename() << "(" << resultInfo.getLine() << "): ";
#else                
                m_config.stream() << resultInfo.getFilename() << ":" << resultInfo.getLine() << ": ";            
#endif            
            
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
                case ResultWas::DidntThrowException:
                    if( resultInfo.hasExpression() )
                        m_config.stream() << " because no exception was thrown where one was expected";
                    else
                        m_config.stream() << "No exception thrown where one was expected";
                    break;
                case ResultWas::Info:
                    streamVariableLengthText( "info", resultInfo.getMessage() );
                    break;
                case ResultWas::Warning:
                    m_config.stream() << "warning:\n'" << resultInfo.getMessage() << "'";
                    break;
                case ResultWas::ExplicitFailure:
                    m_config.stream() << "failed with message: '" << resultInfo.getMessage() << "'";
                    break;
                default:
                    if( !resultInfo.hasExpression() )
                    {
                        if( resultInfo.ok() )
                            m_config.stream() << " succeeded";
                        else
                            m_config.stream() << " failed";
                    }
                    break;
            }
            
            if( resultInfo.hasExpression() )
            {
                m_config.stream() << " for: " << resultInfo.getExpandedExpression();
            }
            m_config.stream() << std::endl;        
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTestCase
        (
            const TestCaseInfo& testInfo, 
            std::size_t succeeded, 
            std::size_t failed, 
            const std::string& stdOut, 
            const std::string& stdErr
        )
        {
            if( !stdOut.empty() )
            {
                StartSpansLazily();
                streamVariableLengthText( "stdout", stdOut );
            }

            if( !stdErr.empty() )
            {
                StartSpansLazily();
                streamVariableLengthText( "stderr", stdErr );
            }
                
            if( m_testSpan.emitted )
            {
                m_config.stream() << "[Finished: " << testInfo.getName() << " ";
                ReportCounts( succeeded, failed );
                m_config.stream() << "]" << std::endl;
            }
        }    

    private: // helpers
        
        ///////////////////////////////////////////////////////////////////////////
        void StartSpansLazily()
        {
            if( !m_testingSpan.emitted )
            {
                if( m_config.getName().empty() )
                    m_config.stream() << "[Started testing]" << std::endl;
                else
                    m_config.stream() << "[Started testing: " << m_config.getName() << "]" << std::endl;
                m_testingSpan.emitted = true;
            }
            
            if( !m_groupSpan.emitted && !m_groupSpan.name.empty() )
            {
                m_config.stream() << "[Started group: '" << m_groupSpan.name << "']" << std::endl;
                m_groupSpan.emitted = true;
            }
            
            if( !m_testSpan.emitted )
            {
                m_config.stream() << std::endl << "[Running: " << m_testSpan.name << "]" << std::endl;
                m_testSpan.emitted = true;
            }
            
            if( !m_sectionSpans.empty() )
            {
                SpanInfo& sectionSpan = m_sectionSpans.back();
                if( !sectionSpan.emitted && !sectionSpan.name.empty() )
                {
                    if( m_firstSectionInTestCase )
                    {
                        m_config.stream() << "\n";
                        m_firstSectionInTestCase = false;
                    }
                    std::vector<SpanInfo>::iterator it = m_sectionSpans.begin();
                    std::vector<SpanInfo>::iterator itEnd = m_sectionSpans.end();
                    for(; it != itEnd; ++it )
                    {
                        SpanInfo& prevSpan = *it;
                        if( !prevSpan.emitted && !prevSpan.name.empty() )
                        {
                            m_config.stream() << "[Started section: '" << prevSpan.name << "']" << std::endl;
                            prevSpan.emitted = true;
                        }                        
                    }
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////
        void streamVariableLengthText
        (
            const std::string& prefix,
            const std::string& text
        )
        {
            std::string trimmed = trim( text );
            if( trimmed.find_first_of( "\r\n" ) == std::string::npos )
            {
                m_config.stream() << "[" << prefix << ": " << trimmed << "]\n";
            }
            else 
            {
                m_config.stream()   << "\n[" << prefix << "] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << trimmed 
                                    << "\n[end of " << prefix << "] <<<<<<<<<<<<<<<<<<<<<<<<\n";
            }
        }
        
    private:
        const IReporterConfig& m_config;
        bool m_firstSectionInTestCase;

        SpanInfo m_testingSpan;
        SpanInfo m_groupSpan;
        SpanInfo m_testSpan;
        std::vector<SpanInfo> m_sectionSpans;
    };

    INTERNAL_CATCH_REGISTER_REPORTER( "basic", BasicReporter )
    
} // end namespace Catch
    
#endif // TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED
