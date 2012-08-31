/*
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED

#include "../internal/catch_capture.hpp"
#include "../internal/catch_interfaces_reporter.h"
#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_xmlwriter.hpp"

namespace Catch {
    class XmlReporter : public SharedImpl<IReporter> {
    public:
        XmlReporter( const ReporterConfig& config ) : m_config( config ) {}

        static std::string getDescription() {
            return "Reports test results as an XML document";
        }
        virtual ~XmlReporter();
        
    private: // IReporter

        virtual bool shouldRedirectStdout() const {
            return true;
        }        

        virtual void StartTesting() {
            m_xml = XmlWriter( m_config.stream );
            m_xml.startElement( "Catch" );
            if( !m_config.name.empty() )
                m_xml.writeAttribute( "name", m_config.name );
        }
        
        virtual void EndTesting( const Totals& totals ) {
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", totals.assertions.passed )
                .writeAttribute( "failures", totals.assertions.failed );
            m_xml.endElement();
        }
        
        virtual void StartGroup( const std::string& groupName ) {
            m_xml.startElement( "Group" )
                .writeAttribute( "name", groupName );
        }

        virtual void EndGroup( const std::string&, const Totals& totals ) {
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", totals.assertions.passed )
                .writeAttribute( "failures", totals.assertions.failed );
            m_xml.endElement();
        }
        
        virtual void StartSection( const std::string& sectionName, const std::string& description ) {
            m_xml.startElement( "Section" )
                .writeAttribute( "name", sectionName )
                .writeAttribute( "description", description );
        }
        virtual void NoAssertionsInSection( const std::string& ) {}
        virtual void NoAssertionsInTestCase( const std::string& ) {}

        virtual void EndSection( const std::string& /*sectionName*/, const Counts& assertions ) {
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", assertions.passed )
                .writeAttribute( "failures", assertions.failed );
            m_xml.endElement();
        }
        
        virtual void StartTestCase( const Catch::TestCaseInfo& testInfo ) {
            m_xml.startElement( "TestCase" ).writeAttribute( "name", testInfo.getName() );
            m_currentTestSuccess = true;
        }
        
        virtual void Result( const Catch::ResultInfo& resultInfo ) {
            if( !m_config.includeSuccessfulResults && resultInfo.getResultType() == ResultWas::Ok )
                return;

            if( resultInfo.hasExpression() ) {
                m_xml.startElement( "Expression" )
                    .writeAttribute( "success", resultInfo.ok() )
                    .writeAttribute( "filename", resultInfo.getFilename() )
                    .writeAttribute( "line", resultInfo.getLine() );
                
                m_xml.scopedElement( "Original" )
                    .writeText( resultInfo.getExpression() );
                m_xml.scopedElement( "Expanded" )
                    .writeText( resultInfo.getExpandedExpression() );
                m_currentTestSuccess &= resultInfo.ok();
            }
            
            switch( resultInfo.getResultType() ) {
                case ResultWas::ThrewException:
                    m_xml.scopedElement( "Exception" )
                        .writeAttribute( "filename", resultInfo.getFilename() )
                        .writeAttribute( "line", resultInfo.getLine() )
                        .writeText( resultInfo.getMessage() );
                    m_currentTestSuccess = false;
                    break;
                case ResultWas::Info:
                    m_xml.scopedElement( "Info" )
                        .writeText( resultInfo.getMessage() );
                    break;
                case ResultWas::Warning:
                    m_xml.scopedElement( "Warning" )
                        .writeText( resultInfo.getMessage() );
                    break;
                case ResultWas::ExplicitFailure:
                    m_xml.scopedElement( "Failure" )
                        .writeText( resultInfo.getMessage() );
                    m_currentTestSuccess = false;
                    break;
                case ResultWas::Unknown:
                case ResultWas::Ok:
                case ResultWas::FailureBit:
                case ResultWas::ExpressionFailed:
                case ResultWas::Exception:
                case ResultWas::DidntThrowException:
                    break;
            }            
            if( resultInfo.hasExpression() )
                m_xml.endElement();
        }

        virtual void Aborted() {
            // !TBD
        }
        
        virtual void EndTestCase( const Catch::TestCaseInfo&, const Totals&, const std::string&, const std::string& ) {
            m_xml.scopedElement( "OverallResult" ).writeAttribute( "success", m_currentTestSuccess );
            m_xml.endElement();
        }    
                
    private:
        ReporterConfig m_config;
        bool m_currentTestSuccess;
        XmlWriter m_xml;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED
