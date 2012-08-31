/*
 *  Created by Phil on 26/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED

#include "../internal/catch_capture.hpp"
#include "../internal/catch_interfaces_reporter.h"
#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_xmlwriter.hpp"

namespace Catch {

    class JunitReporter : public SharedImpl<IReporter> {
    
        struct TestStats {
            std::string m_element;
            std::string m_resultType;
            std::string m_message;
            std::string m_content;
        };
        
        struct TestCaseStats {
        
            TestCaseStats( const std::string& name = std::string() ) :m_name( name ){}
            
            double      m_timeInSeconds;
            std::string m_status;
            std::string m_className;
            std::string m_name;
            std::vector<TestStats> m_testStats;
        };
        
        struct Stats {
        
            Stats( const std::string& name = std::string() )
            :   m_testsCount( 0 ),
                m_failuresCount( 0 ),
                m_disabledCount( 0 ),
                m_errorsCount( 0 ),
                m_timeInSeconds( 0 ),
                m_name( name )
            {}
            
            std::size_t m_testsCount;
            std::size_t m_failuresCount;
            std::size_t m_disabledCount;
            std::size_t m_errorsCount;
            double      m_timeInSeconds;
            std::string m_name;
            
            std::vector<TestCaseStats> m_testCaseStats;
        };
        
    public:
        JunitReporter( const ReporterConfig& config )
        :   m_config( config ),
            m_testSuiteStats( "AllTests" ),
            m_currentStats( &m_testSuiteStats )
        {}
        virtual ~JunitReporter();
        
        static std::string getDescription() {
            return "Reports test results in an XML format that looks like Ant's junitreport target";
        }
        
    private: // IReporter
        
        virtual bool shouldRedirectStdout() const {
            return true;
        }        

        virtual void StartTesting(){}
        
        virtual void StartGroup( const std::string& groupName ) {            
            m_statsForSuites.push_back( Stats( groupName ) );
            m_currentStats = &m_statsForSuites.back();
        }

        virtual void EndGroup( const std::string&, const Totals& totals ) {
            m_currentStats->m_testsCount = totals.assertions.total();
            m_currentStats = &m_testSuiteStats;
        }
        
        virtual void StartSection( const std::string&, const std::string& ){}

        virtual void NoAssertionsInSection( const std::string& ) {}
        virtual void NoAssertionsInTestCase( const std::string& ) {}

        virtual void EndSection( const std::string&, const Counts& ) {}
        
        virtual void StartTestCase( const Catch::TestCaseInfo& testInfo ) {
            m_currentStats->m_testCaseStats.push_back( TestCaseStats( testInfo.getName() ) );            
        }
        
        virtual void Result( const Catch::ResultInfo& resultInfo ) {
            if( resultInfo.getResultType() != ResultWas::Ok || m_config.includeSuccessfulResults ) {
                TestCaseStats& testCaseStats = m_currentStats->m_testCaseStats.back();
                TestStats stats;
                std::ostringstream oss;
                if( !resultInfo.getMessage().empty() )
                    oss << resultInfo.getMessage() << " at ";
                oss << SourceLineInfo( resultInfo.getFilename(), resultInfo.getLine() );
                stats.m_content = oss.str();
                stats.m_message = resultInfo.getExpandedExpression();
                stats.m_resultType = resultInfo.getTestMacroName();

                switch( resultInfo.getResultType() ) {
                    case ResultWas::ThrewException:
                        stats.m_element = "error";
                        m_currentStats->m_errorsCount++;
                        break;
                    case ResultWas::Info:
                        stats.m_element = "info"; // !TBD ?
                        break;
                    case ResultWas::Warning:
                        stats.m_element = "warning"; // !TBD ?
                        break;
                    case ResultWas::ExplicitFailure:
                        stats.m_element = "failure";
                        m_currentStats->m_failuresCount++;
                        break;
                    case ResultWas::ExpressionFailed:
                        stats.m_element = "failure";
                        m_currentStats->m_failuresCount++;
                        break;
                    case ResultWas::Ok:
                        stats.m_element = "success";
                        break;
                    case ResultWas::Unknown:
                    case ResultWas::FailureBit:
                    case ResultWas::Exception:
                    case ResultWas::DidntThrowException:
                        break;
                }
                testCaseStats.m_testStats.push_back( stats );                
            }
        }
        
        virtual void EndTestCase( const Catch::TestCaseInfo&, const Totals&, const std::string& stdOut, const std::string& stdErr ) {
            if( !stdOut.empty() )
                m_stdOut << stdOut << "\n";
            if( !stdErr.empty() )
                m_stdErr << stdErr << "\n";
        }    

        virtual void Aborted() {
            // !TBD
        }

        virtual void EndTesting( const Totals& ) {
            std::ostream& str = m_config.stream;
            {
                XmlWriter xml( str );
                
                if( m_statsForSuites.size() > 0 )
                    xml.startElement( "testsuites" );
                            
                std::vector<Stats>::const_iterator it = m_statsForSuites.begin();
                std::vector<Stats>::const_iterator itEnd = m_statsForSuites.end();
                
                for(; it != itEnd; ++it ) {
                    XmlWriter::ScopedElement e = xml.scopedElement( "testsuite" );
                    xml.writeAttribute( "name", it->m_name );
                    xml.writeAttribute( "errors", it->m_errorsCount );
                    xml.writeAttribute( "failures", it->m_failuresCount );
                    xml.writeAttribute( "tests", it->m_testsCount );
                    xml.writeAttribute( "hostname", "tbd" );
                    xml.writeAttribute( "time", "tbd" );
                    xml.writeAttribute( "timestamp", "tbd" );
                    
                    OutputTestCases( xml, *it );
                }
      
                xml.scopedElement( "system-out" ).writeText( trim( m_stdOut.str() ) );                
                xml.scopedElement( "system-err" ).writeText( trim( m_stdErr.str() ) );
            }
        }
        
        void OutputTestCases( XmlWriter& xml, const Stats& stats ) {
            std::vector<TestCaseStats>::const_iterator it = stats.m_testCaseStats.begin();
            std::vector<TestCaseStats>::const_iterator itEnd = stats.m_testCaseStats.end();
            for(; it != itEnd; ++it ) {
                xml.writeBlankLine();
                xml.writeComment( "Test case" );
                
                XmlWriter::ScopedElement e = xml.scopedElement( "testcase" );
                xml.writeAttribute( "classname", it->m_className );
                xml.writeAttribute( "name", it->m_name );
                xml.writeAttribute( "time", "tbd" );

                OutputTestResult( xml, *it );
            }
        }

        
        void OutputTestResult( XmlWriter& xml, const TestCaseStats& stats ) {
            std::vector<TestStats>::const_iterator it = stats.m_testStats.begin();
            std::vector<TestStats>::const_iterator itEnd = stats.m_testStats.end();
            for(; it != itEnd; ++it ) {
                if( it->m_element != "success" ) {
                    XmlWriter::ScopedElement e = xml.scopedElement( it->m_element );
                    
                    xml.writeAttribute( "message", it->m_message );
                    xml.writeAttribute( "type", it->m_resultType );
                    if( !it->m_content.empty() )
                        xml.writeText( it->m_content );
                }
            }
        }
        
    private:
        ReporterConfig m_config;
        bool m_currentTestSuccess;
        
        Stats m_testSuiteStats;
        Stats* m_currentStats;
        std::vector<Stats> m_statsForSuites;
        std::ostringstream m_stdOut;
        std::ostringstream m_stdErr;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED
