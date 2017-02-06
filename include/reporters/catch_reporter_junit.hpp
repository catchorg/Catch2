/*
 *  Created by Phil on 26/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED

#include "catch_reporter_bases.hpp"

#include "../internal/catch_tostring.h"
#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_xmlwriter.hpp"

#include <assert.h>

namespace Catch {

    namespace {
        std::string getCurrentTimestamp() {
            // Beware, this is not reentrant because of backward compatibility issues
            // Also, UTC only, again because of backward compatibility (%z is C++11)
            time_t rawtime;
            std::time(&rawtime);
            const size_t timeStampSize = sizeof("2017-01-16T17:06:45Z");

#ifdef _MSC_VER
            std::tm timeInfo = {};
            gmtime_s(&timeInfo, &rawtime);
#else
            std::tm* timeInfo;
            timeInfo = std::gmtime(&rawtime);
#endif

            char timeStamp[timeStampSize];
            const char * const fmt = "%Y-%m-%dT%H:%M:%SZ";

#ifdef _MSC_VER
            std::strftime(timeStamp, timeStampSize, fmt, &timeInfo);
#else
            std::strftime(timeStamp, timeStampSize, fmt, timeInfo);
#endif
            return std::string(timeStamp);
        }

    }

    class JunitReporter : public CumulativeReporterBase {
    public:
        JunitReporter( ReporterConfig const& _config )
        :   CumulativeReporterBase( _config ),
            xml( _config.stream() )
        {
            m_reporterPrefs.shouldRedirectStdOut = true;
        }

        virtual ~JunitReporter() CATCH_OVERRIDE;

        static std::string getDescription() {
            return "Reports test results in an XML format that looks like Ant's junitreport target";
        }

        virtual void noMatchingTestCases( std::string const& /*spec*/ ) CATCH_OVERRIDE {}

        virtual void testRunStarting( TestRunInfo const& runInfo ) CATCH_OVERRIDE {
            CumulativeReporterBase::testRunStarting( runInfo );
            xml.startElement( "testsuites" );
        }

        virtual void testGroupStarting( GroupInfo const& groupInfo ) CATCH_OVERRIDE {
            suiteTimer.start();
            stdOutForSuite.str("");
            stdErrForSuite.str("");
            unexpectedExceptions = 0;
            CumulativeReporterBase::testGroupStarting( groupInfo );
        }

        virtual bool assertionEnded( AssertionStats const& assertionStats ) CATCH_OVERRIDE {
            if( assertionStats.assertionResult.getResultType() == ResultWas::ThrewException )
                unexpectedExceptions++;
            return CumulativeReporterBase::assertionEnded( assertionStats );
        }

        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) CATCH_OVERRIDE {
            stdOutForSuite << testCaseStats.stdOut;
            stdErrForSuite << testCaseStats.stdErr;
            CumulativeReporterBase::testCaseEnded( testCaseStats );
        }

        virtual void testGroupEnded( TestGroupStats const& testGroupStats ) CATCH_OVERRIDE {
            double suiteTime = suiteTimer.getElapsedSeconds();
            CumulativeReporterBase::testGroupEnded( testGroupStats );
            writeGroup( *m_testGroups.back(), suiteTime );
        }

        virtual void testRunEndedCumulative() CATCH_OVERRIDE {
            xml.endElement();
        }

        void writeGroup( TestGroupNode const& groupNode, double suiteTime ) {
            XmlWriter::ScopedElement e = xml.scopedElement( "testsuite" );
            TestGroupStats const& stats = groupNode.value;
            xml.writeAttribute( "name", stats.groupInfo.name );
            xml.writeAttribute( "errors", unexpectedExceptions );
            xml.writeAttribute( "failures", stats.totals.assertions.failed-unexpectedExceptions );
            xml.writeAttribute( "tests", stats.totals.assertions.total() );
            xml.writeAttribute( "hostname", "tbd" ); // !TBD
            if( m_config->showDurations() == ShowDurations::Never )
                xml.writeAttribute( "time", "" );
            else
                xml.writeAttribute( "time", suiteTime );
            xml.writeAttribute( "timestamp", getCurrentTimestamp() );

            // Write test cases
            for( TestGroupNode::ChildNodes::const_iterator
                    it = groupNode.children.begin(), itEnd = groupNode.children.end();
                    it != itEnd;
                    ++it )
                writeTestCase( **it );

            xml.scopedElement( "system-out" ).writeText( trim( stdOutForSuite.str() ), false );
            xml.scopedElement( "system-err" ).writeText( trim( stdErrForSuite.str() ), false );
        }

        void writeTestCase( TestCaseNode const& testCaseNode ) {
            TestCaseStats const& stats = testCaseNode.value;

            // All test cases have exactly one section - which represents the
            // test case itself. That section may have 0-n nested sections
            assert( testCaseNode.children.size() == 1 );
            SectionNode const& rootSection = *testCaseNode.children.front();

            std::string className = stats.testInfo.className;

            if( className.empty() ) {
                if( rootSection.childSections.empty() )
                    className = "global";
            }
            writeSection( className, "", rootSection );
        }

        void writeSection(  std::string const& className,
                            std::string const& rootName,
                            SectionNode const& sectionNode ) {
            std::string name = trim( sectionNode.stats.sectionInfo.name );
            if( !rootName.empty() )
                name = rootName + '/' + name;

            if( !sectionNode.assertions.empty() ||
                !sectionNode.stdOut.empty() ||
                !sectionNode.stdErr.empty() ) {
                XmlWriter::ScopedElement e = xml.scopedElement( "testcase" );
                if( className.empty() ) {
                    xml.writeAttribute( "classname", name );
                    xml.writeAttribute( "name", "root" );
                }
                else {
                    xml.writeAttribute( "classname", className );
                    xml.writeAttribute( "name", name );
                }
                xml.writeAttribute( "time", Catch::toString( sectionNode.stats.durationInSeconds ) );

                writeAssertions( sectionNode );

                if( !sectionNode.stdOut.empty() )
                    xml.scopedElement( "system-out" ).writeText( trim( sectionNode.stdOut ), false );
                if( !sectionNode.stdErr.empty() )
                    xml.scopedElement( "system-err" ).writeText( trim( sectionNode.stdErr ), false );
            }
            for( SectionNode::ChildSections::const_iterator
                    it = sectionNode.childSections.begin(),
                    itEnd = sectionNode.childSections.end();
                    it != itEnd;
                    ++it )
                if( className.empty() )
                    writeSection( name, "", **it );
                else
                    writeSection( className, name, **it );
        }

        void writeAssertions( SectionNode const& sectionNode ) {
            for( SectionNode::Assertions::const_iterator
                    it = sectionNode.assertions.begin(), itEnd = sectionNode.assertions.end();
                    it != itEnd;
                    ++it )
                writeAssertion( *it );
        }
        void writeAssertion( AssertionStats const& stats ) {
            AssertionResult const& result = stats.assertionResult;
            if( !result.isOk() ) {
                std::string elementName;
                switch( result.getResultType() ) {
                    case ResultWas::ThrewException:
                    case ResultWas::FatalErrorCondition:
                        elementName = "error";
                        break;
                    case ResultWas::ExplicitFailure:
                        elementName = "failure";
                        break;
                    case ResultWas::ExpressionFailed:
                        elementName = "failure";
                        break;
                    case ResultWas::DidntThrowException:
                        elementName = "failure";
                        break;

                    // We should never see these here:
                    case ResultWas::Info:
                    case ResultWas::Warning:
                    case ResultWas::Ok:
                    case ResultWas::Unknown:
                    case ResultWas::FailureBit:
                    case ResultWas::Exception:
                        elementName = "internalError";
                        break;
                }

                XmlWriter::ScopedElement e = xml.scopedElement( elementName );

                xml.writeAttribute( "message", result.getExpandedExpression() );
                xml.writeAttribute( "type", result.getTestMacroName() );

                std::ostringstream oss;
                if( !result.getMessage().empty() )
                    oss << result.getMessage() << '\n';
                for( std::vector<MessageInfo>::const_iterator
                        it = stats.infoMessages.begin(),
                        itEnd = stats.infoMessages.end();
                            it != itEnd;
                            ++it )
                    if( it->type == ResultWas::Info )
                        oss << it->message << '\n';

                oss << "at " << result.getSourceInfo();
                xml.writeText( oss.str(), false );
            }
        }

        XmlWriter xml;
        Timer suiteTimer;
        std::ostringstream stdOutForSuite;
        std::ostringstream stdErrForSuite;
        unsigned int unexpectedExceptions;
    };

    INTERNAL_CATCH_REGISTER_REPORTER( "junit", JunitReporter )

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED
