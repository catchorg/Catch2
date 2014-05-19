/*
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED

#include "catch_reporter_bases.hpp"

#include "../internal/catch_capture.hpp"
#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_xmlwriter.hpp"

namespace Catch {
    class XmlReporter : public SharedImpl<IReporter> {
    public:
        XmlReporter( ReporterConfig const& config ) : m_config( config ), m_sectionDepth( 0 ) {}

        static std::string getDescription() {
            return "Reports test results as an XML document";
        }
        virtual ~XmlReporter();

    private: // IReporter

        virtual bool shouldRedirectStdout() const {
            return true;
        }

        virtual void StartTesting() {
            m_xml.setStream( m_config.stream() );
            m_xml.startElement( "Catch" );
            if( !m_config.fullConfig()->name().empty() )
                m_xml.writeAttribute( "name", m_config.fullConfig()->name() );
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
            if( m_sectionDepth++ > 0 ) {
                m_xml.startElement( "Section" )
                    .writeAttribute( "name", trim( sectionName ) )
                    .writeAttribute( "description", description );
            }
        }
        virtual void NoAssertionsInSection( const std::string& ) {}
        virtual void NoAssertionsInTestCase( const std::string& ) {}

        virtual void EndSection( const std::string& /*sectionName*/, const Counts& assertions ) {
            if( --m_sectionDepth > 0 ) {
                m_xml.scopedElement( "OverallResults" )
                    .writeAttribute( "successes", assertions.passed )
                    .writeAttribute( "failures", assertions.failed );
                m_xml.endElement();
            }
        }

        virtual void StartTestCase( const Catch::TestCaseInfo& testInfo ) {
            m_xml.startElement( "TestCase" ).writeAttribute( "name", trim( testInfo.name ) );
            m_currentTestSuccess = true;
        }

        virtual void Result( const Catch::AssertionResult& assertionResult ) {
            if( !m_config.fullConfig()->includeSuccessfulResults() && assertionResult.getResultType() == ResultWas::Ok )
                return;

            if( assertionResult.hasExpression() ) {
                m_xml.startElement( "Expression" )
                    .writeAttribute( "success", assertionResult.succeeded() )
                    .writeAttribute( "filename", assertionResult.getSourceInfo().file )
                    .writeAttribute( "line", assertionResult.getSourceInfo().line );

                m_xml.scopedElement( "Original" )
                    .writeText( assertionResult.getExpression() );
                m_xml.scopedElement( "Expanded" )
                    .writeText( assertionResult.getExpandedExpression() );
                m_currentTestSuccess &= assertionResult.succeeded();
            }

            switch( assertionResult.getResultType() ) {
                case ResultWas::ThrewException:
                    m_xml.scopedElement( "Exception" )
                        .writeAttribute( "filename", assertionResult.getSourceInfo().file )
                        .writeAttribute( "line", assertionResult.getSourceInfo().line )
                        .writeText( assertionResult.getMessage() );
                    m_currentTestSuccess = false;
                    break;
                case ResultWas::Info:
                    m_xml.scopedElement( "Info" )
                        .writeText( assertionResult.getMessage() );
                    break;
                case ResultWas::Warning:
                    m_xml.scopedElement( "Warning" )
                        .writeText( assertionResult.getMessage() );
                    break;
                case ResultWas::ExplicitFailure:
                    m_xml.scopedElement( "Failure" )
                        .writeText( assertionResult.getMessage() );
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
            if( assertionResult.hasExpression() )
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
        int m_sectionDepth;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED
