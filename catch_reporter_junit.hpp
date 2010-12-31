/*
 *  catch_reporter_junit.hpp
 *  Catch
 *
 *  Created by Phil on 26/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED

#include "internal/catch_capture.hpp"
#include "internal/catch_interfaces_reporter.h"
#include "internal/catch_reporter_registrars.hpp"
#include "internal/catch_xmlwriter.hpp"

namespace Catch
{
    class JunitReporter : public Catch::IReporter
    {
        struct TestStats
        {
            std::string element;
            std::string resultType;
            std::string message;
            std::string content;
        };
        
        struct TestCaseStats
        {
            TestCaseStats( const std::string& name = std::string() )
            :   name( name )
            {
            }
            
            double      timeInSeconds;
            std::string status;
            std::string className;
            std::string name;
            std::vector<TestStats> testStats;
        };
        
        struct Stats
        {
            Stats( const std::string& name = std::string() )
            :   testsCount( 0 ),
                failuresCount( 0 ),
                disabledCount( 0 ),
                errorsCount( 0 ),
                timeInSeconds( 0 ),
                name( name )
            {
            }
            
            std::size_t testsCount;
            std::size_t failuresCount;
            std::size_t disabledCount;
            std::size_t errorsCount;
            double      timeInSeconds;
            std::string name;
            
            std::vector<TestCaseStats> testCaseStats;
        };
        
    public:
        ///////////////////////////////////////////////////////////////////////////
        JunitReporter( const IReporterConfig& config )
        :   m_config( config ),
            m_testSuiteStats( "AllTests" ),
            m_currentStats( &m_testSuiteStats )
        {
        }        
        
        ///////////////////////////////////////////////////////////////////////////
        static std::string getDescription()
        {
            return "Reports test results in an XML format that looks like Ant's junitreport target";
        }
        
    private: // IReporter
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTesting()
        {
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartGroup( const std::string& groupName )
        {
            
            m_statsForSuites.push_back( Stats( groupName ) );
            m_currentStats = &m_statsForSuites.back();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndGroup( const std::string&, std::size_t succeeded, std::size_t failed )
        {
            m_currentStats->testsCount = failed+succeeded;
            m_currentStats = &m_testSuiteStats;
        }
        
        virtual void StartSection( const std::string& /*sectionName*/, const std::string /*description*/ )
        {
        }

        virtual void EndSection( const std::string& /*sectionName*/, std::size_t /*succeeded*/, std::size_t /*failed*/ )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTestCase( const Catch::TestCaseInfo& testInfo )
        {
            m_currentStats->testCaseStats.push_back( TestCaseStats( testInfo.getName() ) );
            
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void Result( const Catch::ResultInfo& resultInfo )
        {
            if( resultInfo.getResultType() != ResultWas::Ok || m_config.includeSuccessfulResults() )
            {
                TestCaseStats& testCaseStats = m_currentStats->testCaseStats.back();
                TestStats stats;
                std::ostringstream oss;
                if( !resultInfo.getMessage().empty() )
                {
                    oss << resultInfo.getMessage() << " at ";
                }
                oss << resultInfo.getFilename() << ":" << resultInfo.getLine();
                stats.content = oss.str();
                stats.message = resultInfo.getExpandedExpression();
                stats.resultType = resultInfo.getTestMacroName();
                switch( resultInfo.getResultType() )
                {
                    case ResultWas::ThrewException:
                        stats.element = "error";
                        m_currentStats->errorsCount++;
                        break;
                    case ResultWas::Info:
                        stats.element = "info"; // !TBD ?
                        break;
                    case ResultWas::Warning:
                        stats.element = "warning"; // !TBD ?
                        break;
                    case ResultWas::ExplicitFailure:
                        stats.element = "failure";
                        m_currentStats->failuresCount++;
                        break;
                    case ResultWas::ExpressionFailed:
                        stats.element = "failure";
                        m_currentStats->failuresCount++;
                        break;
                    case ResultWas::Ok:
                        stats.element = "success";
                        break;
                    default:
                        stats.element = "unknown";
                        break;
                }
                testCaseStats.testStats.push_back( stats );
                
            }
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTestCase( const Catch::TestCaseInfo&, const std::string& stdOut, const std::string& stdErr )
        {
            if( !stdOut.empty() )
                m_stdOut << stdOut << "\n";
            if( !stdErr.empty() )
                m_stdErr << stdErr << "\n";
        }    

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTesting( std::size_t /* succeeded */, std::size_t /* failed */ )
        {
            std::ostream& str = m_config.stream();
            {
                XmlWriter xml( str );
                
                if( m_statsForSuites.size() > 0 )
                    xml.startElement( "testsuites" );
                            
                std::vector<Stats>::const_iterator it = m_statsForSuites.begin();
                std::vector<Stats>::const_iterator itEnd = m_statsForSuites.end();
                
                for(; it != itEnd; ++it )
                {
                    XmlWriter::ScopedElement e = xml.scopedElement( "testsuite" );
                    xml.writeAttribute( "name", it->name );
                    xml.writeAttribute( "errors", it->errorsCount );
                    xml.writeAttribute( "failures", it->failuresCount );
                    xml.writeAttribute( "tests", it->testsCount );
                    xml.writeAttribute( "hostname", "tbd" );
                    xml.writeAttribute( "time", "tbd" );
                    xml.writeAttribute( "timestamp", "tbd" );
                    
                    OutputTestCases( xml, *it );
                }
      
                xml.scopedElement( "system-out" ).writeText( trim( m_stdOut.str() ) );                
                xml.scopedElement( "system-err" ).writeText( trim( m_stdOut.str() ) );
            }
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void OutputTestCases( XmlWriter& xml, const Stats& stats )
        {
            std::vector<TestCaseStats>::const_iterator it = stats.testCaseStats.begin();
            std::vector<TestCaseStats>::const_iterator itEnd = stats.testCaseStats.end();
            for(; it != itEnd; ++it )
            {
                xml.writeBlankLine();
                xml.writeComment( "Test case" );
                
                XmlWriter::ScopedElement e = xml.scopedElement( "testcase" );
                xml.writeAttribute( "classname", it->className );
                xml.writeAttribute( "name", it->name );
                xml.writeAttribute( "time", "tbd" );

                OutputTestResult( xml, *it );
            }
        }

        
        ///////////////////////////////////////////////////////////////////////////
        void OutputTestResult( XmlWriter& xml, const TestCaseStats& stats )
        {
            std::vector<TestStats>::const_iterator it = stats.testStats.begin();
            std::vector<TestStats>::const_iterator itEnd = stats.testStats.end();
            for(; it != itEnd; ++it )
            {
                if( it->element != "success" )
                {
                    XmlWriter::ScopedElement e = xml.scopedElement( it->element );
                    
                    xml.writeAttribute( "message", it->message );
                    xml.writeAttribute( "type", it->resultType );
                    if( !it->content.empty() )
                        xml.writeText( it->content );
                }
            }
        }
        
    private:
        const IReporterConfig& m_config;
        bool m_currentTestSuccess;
        
        Stats m_testSuiteStats;
        Stats* m_currentStats;
        std::vector<Stats> m_statsForSuites;
        std::ostringstream m_stdOut;
        std::ostringstream m_stdErr;
    };

    CATCH_REGISTER_REPORTER( "junit", JunitReporter );
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED
