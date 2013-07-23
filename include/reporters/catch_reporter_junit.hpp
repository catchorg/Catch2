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

#include <assert.h>

namespace Catch {

    class JunitReporter : public SharedImpl<AccumulatingReporterBase> {
    public:
        JunitReporter( ReporterConfig const& config ) : m_config( config ) {}
        virtual ~JunitReporter();

        static std::string getDescription() {
            return "Reports test results in an XML format that looks like Ant's junitreport target";
        }

        virtual ReporterPreferences getPreferences() const {
            ReporterPreferences prefs;
            prefs.shouldRedirectStdOut = true;
            return prefs;
        }

        virtual void noMatchingTestCases( std::string const& spec ) {
            (void)spec;
        }

        virtual void processResults( AccumTestRunStats const& stats ) {
            XmlWriter xml( m_config.stream() );
            OutputTestSuites( xml, stats );
        }

    private:
        static void OutputTestSuites( XmlWriter& xml, AccumTestRunStats const& stats ) {
            xml.startElement( "testsuites" );
            xml.writeAttribute( "name", stats.testRun.runInfo.name );
            xml.writeAttribute( "time", stats.testRun.timeSecs );

            std::vector<AccumTestGroupStats>::const_iterator it = stats.testGroups.begin();
            std::vector<AccumTestGroupStats>::const_iterator itEnd = stats.testGroups.end();

            std::ostringstream stdErr, stdOut;
            for( ; it != itEnd; ++it ) {
                OutputTestSuite( xml, *it);
                CollectErrAndOutMessages( *it, stdErr, stdOut );
            }

            OutputTextIfNotEmpty( xml, "system-out", stdOut.str() );
            OutputTextIfNotEmpty( xml, "system-err", stdErr.str() );
        }

        static void OutputTestSuite( XmlWriter& xml, AccumTestGroupStats const& stats ) {
            size_t errors = 0, failures = 0;
            CountErrorAndFailures(stats, errors, failures);

            XmlWriter::ScopedElement e = xml.scopedElement( "testsuite" );
            xml.writeAttribute( "name", stats.testGroup.groupInfo.name );
            xml.writeAttribute( "errors", errors );
            xml.writeAttribute( "failures", failures );
            xml.writeAttribute( "tests", stats.testCases.size() );
            xml.writeAttribute( "hostname", "tbd" );
            xml.writeAttribute( "time", stats.testGroup.timeSecs );
            xml.writeAttribute( "timestamp", "tbd" );

            std::vector<AccumTestCaseStats>::const_iterator it2 = stats.testCases.begin();
            std::vector<AccumTestCaseStats>::const_iterator it2End = stats.testCases.end();
            for(; it2 != it2End; ++it2 ) {
                OutputTestCase( xml, *it2 );
            }
        }

        static void OutputTestCase( XmlWriter& xml, AccumTestCaseStats const& stats ) {
            XmlWriter::ScopedElement e = xml.scopedElement( "testcase" );
            xml.writeAttribute( "classname", stats.testCase.testInfo.className );
            xml.writeAttribute( "name", stats.testCase.testInfo.name );
            xml.writeAttribute( "time", stats.testCase.timeSecs );

            std::vector<AssertionStats>::const_iterator it = stats.tests.begin();
            std::vector<AssertionStats>::const_iterator itEnd = stats.tests.end();
            for( ; it != itEnd; ++it ) {
                OutputTestResult( xml, *it );
            }

            OutputTextIfNotEmpty( xml, "system-out", stats.testCase.stdOut );
            OutputTextIfNotEmpty( xml, "system-err", stats.testCase.stdErr );
        }

        static std::string GetResultTag( AssertionStats const& test ) {
            switch(test.assertionResult.getResultType()) {
                case ResultWas::Ok:                     return "success";
                case ResultWas::ThrewException:         return "error";
                case ResultWas::Info:                   return "info";
                case ResultWas::Warning:                return "warning";
                case ResultWas::ExplicitFailure:
                case ResultWas::ExpressionFailed:
                case ResultWas::DidntThrowException:    return "failure";
                case ResultWas::Unknown:
                case ResultWas::FailureBit:
                case ResultWas::Exception:
                default:                                return "* internal error *";
            }
        }

        static void OutputTestResult( XmlWriter& xml, AssertionStats const& test ) {
            std::string tag = GetResultTag(test);
            if( tag != "success" ) {
                XmlWriter::ScopedElement e = xml.scopedElement( tag );

                xml.writeAttribute( "message", test.assertionResult.getExpandedExpression() );
                xml.writeAttribute( "type", test.assertionResult.getTestMacroName() );

                std::ostringstream oss;
                if( !test.assertionResult.getMessage().empty() ) {
                    oss << test.assertionResult.getMessage() << " at ";
                }
                oss << test.assertionResult.getSourceInfo();
                xml.writeText( oss.str() );
            }
        }

        static void OutputTextIfNotEmpty( XmlWriter& xml, std::string const& elementName, std::string const& text ) {
            std::string trimmed = trim( text );
            if( !trimmed.empty() ) {
                xml.scopedElement( elementName ).writeText( trimmed, false );
            }
        }

        static void CountErrorAndFailures(AccumTestGroupStats const& stats, size_t& outErrors, size_t& outFailures) {
            std::vector<AccumTestCaseStats>::const_iterator it = stats.testCases.begin();
            std::vector<AccumTestCaseStats>::const_iterator itEnd = stats.testCases.end();
            for( ; it != itEnd; ++it ) {
                std::vector<AssertionStats>::const_iterator it2 = it->tests.begin();
                std::vector<AssertionStats>::const_iterator it2End = it->tests.end();
                for( ; it2 != it2End; ++it2 ) {
                    std::string tag = GetResultTag(*it2);
                    if( tag == "error" )   { ++outErrors;   }
                    if( tag == "failure" ) { ++outFailures; }
                }
            }
        }

        static void CollectErrAndOutMessages(AccumTestGroupStats const& stats, std::ostream& outErr, std::ostream& outOut) {
            std::vector<AccumTestCaseStats>::const_iterator it = stats.testCases.begin();
            std::vector<AccumTestCaseStats>::const_iterator itEnd = stats.testCases.end();
            for( ; it != itEnd; ++it ) {
                std::string err = trim( it->testCase.stdErr );
                if( !err.empty() ) { outErr << err << std::endl; }
                std::string out = trim( it->testCase.stdOut );
                if( !out.empty() ) { outOut << out << std::endl; }
            }
        }

    private:
        ReporterConfig m_config;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED
