/*
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_reporter_bases.hpp"

#include "../internal/catch_capture.hpp"
#include "../internal/catch_reporter_registrars.hpp"
#include "internal/catch_xmlwriter.h"
#include "../internal/catch_timer.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4061) // Not all labels are EXPLICITLY handled in switch
                              // Note that 4062 (not all labels are handled
                              // and default is missing) is enabled
#endif

namespace Catch {
    class XmlReporter : public StreamingReporterBase<XmlReporter> {
    public:
        XmlReporter( ReporterConfig const& _config )
        :   StreamingReporterBase( _config ),
            m_xml(_config.stream())
        {
            m_reporterPrefs.shouldRedirectStdOut = true;
        }

        ~XmlReporter() override;

        static std::string getDescription() {
            return "Reports test results as an XML document";
        }

        virtual std::string getStylesheetRef() const {
            return std::string();
        }

        void writeSourceInfo( SourceLineInfo const& sourceInfo ) {
            m_xml
                .writeAttribute( "filename", sourceInfo.file )
                .writeAttribute( "line", sourceInfo.line );
        }

    public: // StreamingReporterBase

        void noMatchingTestCases( std::string const& s ) override {
            StreamingReporterBase::noMatchingTestCases( s );
        }

        void testRunStarting( TestRunInfo const& testInfo ) override {
            StreamingReporterBase::testRunStarting( testInfo );
            std::string stylesheetRef = getStylesheetRef();
            if( !stylesheetRef.empty() )
                m_xml.writeStylesheetRef( stylesheetRef );
            m_xml.startElement( "Catch" );
            if( !m_config->name().empty() )
                m_xml.writeAttribute( "name", m_config->name() );
        }

        void testGroupStarting( GroupInfo const& groupInfo ) override {
            StreamingReporterBase::testGroupStarting( groupInfo );
            m_xml.startElement( "Group" )
                .writeAttribute( "name", groupInfo.name );
        }

        void testCaseStarting( TestCaseInfo const& testInfo ) override {
            StreamingReporterBase::testCaseStarting(testInfo);
            m_xml.startElement( "TestCase" )
                .writeAttribute( "name", trim( testInfo.name ) )
                .writeAttribute( "description", testInfo.description )
                .writeAttribute( "tags", testInfo.tagsAsString() );

            writeSourceInfo( testInfo.lineInfo );

            if ( m_config->showDurations() == ShowDurations::Always )
                m_testCaseTimer.start();
            m_xml.ensureTagClosed();
        }

        void sectionStarting( SectionInfo const& sectionInfo ) override {
            StreamingReporterBase::sectionStarting( sectionInfo );
            if( m_sectionDepth++ > 0 ) {
                m_xml.startElement( "Section" )
                    .writeAttribute( "name", trim( sectionInfo.name ) )
                    .writeAttribute( "description", sectionInfo.description );
                writeSourceInfo( sectionInfo.lineInfo );
                m_xml.ensureTagClosed();
            }
        }

        void assertionStarting( AssertionInfo const& ) override { }

        bool assertionEnded( AssertionStats const& assertionStats ) override {

            AssertionResult const& result = assertionStats.assertionResult;

            bool includeResults = m_config->includeSuccessfulResults() || !result.isOk();

            if( includeResults ) {
                // Print any info messages in <Info> tags.
                for( auto const& msg : assertionStats.infoMessages ) {
                    if( msg.type == ResultWas::Info ) {
                        m_xml.scopedElement( "Info" )
                                .writeText( msg.message );
                    } else if ( msg.type == ResultWas::Warning ) {
                        m_xml.scopedElement( "Warning" )
                                .writeText( msg.message );
                    }
                }
            }

            // Drop out if result was successful but we're not printing them.
            if( !includeResults && result.getResultType() != ResultWas::Warning )
                return true;


            // Print the expression if there is one.
            if( result.hasExpression() ) {
                m_xml.startElement( "Expression" )
                    .writeAttribute( "success", result.succeeded() )
                    .writeAttribute( "type", result.getTestMacroName() );

                writeSourceInfo( result.getSourceInfo() );

                m_xml.scopedElement( "Original" )
                    .writeText( result.getExpression() );
                m_xml.scopedElement( "Expanded" )
                    .writeText( result.getExpandedExpression() );
            }

            // And... Print a result applicable to each result type.
            switch( result.getResultType() ) {
                case ResultWas::ThrewException:
                    m_xml.startElement( "Exception" );
                    writeSourceInfo( result.getSourceInfo() );
                    m_xml.writeText( result.getMessage() );
                    m_xml.endElement();
                    break;
                case ResultWas::FatalErrorCondition:
                    m_xml.startElement( "FatalErrorCondition" );
                    writeSourceInfo( result.getSourceInfo() );
                    m_xml.writeText( result.getMessage() );
                    m_xml.endElement();
                    break;
                case ResultWas::Info:
                    m_xml.scopedElement( "Info" )
                        .writeText( result.getMessage() );
                    break;
                case ResultWas::Warning:
                    // Warning will already have been written
                    break;
                case ResultWas::ExplicitFailure:
                    m_xml.startElement( "Failure" );
                    writeSourceInfo( result.getSourceInfo() );
                    m_xml.writeText( result.getMessage() );
                    m_xml.endElement();
                    break;
                default:
                    break;
            }

            if( result.hasExpression() )
                m_xml.endElement();

            return true;
        }

        void sectionEnded( SectionStats const& sectionStats ) override {
            StreamingReporterBase::sectionEnded( sectionStats );
            if( --m_sectionDepth > 0 ) {
                XmlWriter::ScopedElement e = m_xml.scopedElement( "OverallResults" );
                e.writeAttribute( "successes", sectionStats.assertions.passed );
                e.writeAttribute( "failures", sectionStats.assertions.failed );
                e.writeAttribute( "expectedFailures", sectionStats.assertions.failedButOk );

                if ( m_config->showDurations() == ShowDurations::Always )
                    e.writeAttribute( "durationInSeconds", sectionStats.durationInSeconds );

                m_xml.endElement();
            }
        }

        void testCaseEnded( TestCaseStats const& testCaseStats ) override {
            StreamingReporterBase::testCaseEnded( testCaseStats );
            XmlWriter::ScopedElement e = m_xml.scopedElement( "OverallResult" );
            e.writeAttribute( "success", testCaseStats.totals.assertions.allOk() );

            if ( m_config->showDurations() == ShowDurations::Always )
                e.writeAttribute( "durationInSeconds", m_testCaseTimer.getElapsedSeconds() );

            if( !testCaseStats.stdOut.empty() )
                m_xml.scopedElement( "StdOut" ).writeText( trim( testCaseStats.stdOut ), false );
            if( !testCaseStats.stdErr.empty() )
                m_xml.scopedElement( "StdErr" ).writeText( trim( testCaseStats.stdErr ), false );

            m_xml.endElement();
        }

        void testGroupEnded( TestGroupStats const& testGroupStats ) override {
            StreamingReporterBase::testGroupEnded( testGroupStats );
            // TODO: Check testGroupStats.aborting and act accordingly.
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", testGroupStats.totals.assertions.passed )
                .writeAttribute( "failures", testGroupStats.totals.assertions.failed )
                .writeAttribute( "expectedFailures", testGroupStats.totals.assertions.failedButOk );
            m_xml.endElement();
        }

        void testRunEnded( TestRunStats const& testRunStats ) override {
            StreamingReporterBase::testRunEnded( testRunStats );
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", testRunStats.totals.assertions.passed )
                .writeAttribute( "failures", testRunStats.totals.assertions.failed )
                .writeAttribute( "expectedFailures", testRunStats.totals.assertions.failedButOk );
            m_xml.endElement();
        }

    private:
        Timer m_testCaseTimer;
        XmlWriter m_xml;
        int m_sectionDepth = 0;
    };

    XmlReporter::~XmlReporter() {}
    CATCH_REGISTER_REPORTER( "xml", XmlReporter )

} // end namespace Catch

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
