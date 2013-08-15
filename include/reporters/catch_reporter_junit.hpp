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

    class JunitReporter : public CumulativeReporterBase {
    public:
        JunitReporter( ReporterConfig const& _config )
        :   CumulativeReporterBase( _config ),
            xml( _config.stream() )
        {}

        ~JunitReporter();

        static std::string getDescription() {
            return "Reports test results in an XML format that looks like Ant's junitreport target";
        }

        virtual void noMatchingTestCases( std::string const& /*spec*/ ) {}

        virtual ReporterPreferences getPreferences() const {
            ReporterPreferences prefs;
            prefs.shouldRedirectStdOut = true;
            return prefs;
        }

        virtual void testRunStarting( TestRunInfo const& runInfo ) {
            CumulativeReporterBase::testRunStarting( runInfo );
            xml.startElement( "testsuites" );
        }

        virtual void testGroupStarting( GroupInfo const& groupInfo ) {
            suiteTimer.start();
            stdOutForSuite.str("");
            stdErrForSuite.str("");
            unexpectedExceptions = 0;
            CumulativeReporterBase::testGroupStarting( groupInfo );
        }

        virtual bool assertionEnded( AssertionStats const& assertionStats ) {
            if( assertionStats.assertionResult.getResultType() == ResultWas::ThrewException )
                unexpectedExceptions++;
            return CumulativeReporterBase::assertionEnded( assertionStats );
        }

        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) {
            stdOutForSuite << testCaseStats.stdOut;
            stdErrForSuite << testCaseStats.stdErr;
            CumulativeReporterBase::testCaseEnded( testCaseStats );
        }

        virtual void testGroupEnded( TestGroupStats const& testGroupStats ) {
            double suiteTime = suiteTimer.getElapsedSeconds();
            CumulativeReporterBase::testGroupEnded( testGroupStats );
            writeGroup( *m_testGroups.back(), suiteTime );
        }

        virtual void testRunEnded() {
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
            xml.writeAttribute( "timestamp", "tbd" ); // !TBD

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
                name = rootName + "/" + name;
            
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
                xml.writeAttribute( "time", toString( sectionNode.stats.durationInSeconds ) );

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
                    oss << result.getMessage() << "\n";
                for( std::vector<MessageInfo>::const_iterator
                        it = stats.infoMessages.begin(),
                        itEnd = stats.infoMessages.end();
                            it != itEnd;
                            ++it )
                    if( it->type == ResultWas::Info )
                        oss << it->message << "\n";

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

    class JunitReporter2 : public SharedImpl<IReporter> {

        struct TestStats {
            std::string m_element;
            std::string m_resultType;
            std::string m_message;
            std::string m_content;
        };

        struct TestCaseStats {

            TestCaseStats( const std::string& className, const std::string& name )
            :   m_className( className ),
                m_name( name )
            {}

            double      m_timeInSeconds;
            std::string m_status;
            std::string m_className;
            std::string m_name;
            std::string m_stdOut;
            std::string m_stdErr;
            std::vector<TestStats> m_testStats;
            std::vector<TestCaseStats> m_sections;
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
        JunitReporter2( ReporterConfig const& config )
        :   m_config( config ),
            m_testSuiteStats( "AllTests" ),
            m_currentStats( &m_testSuiteStats )
        {}
//        virtual ~JunitReporter2();

        static std::string getDescription() {
            return "Reports test results in an XML format that looks like Ant's junitreport target";
        }

    private: // IReporter

        virtual bool shouldRedirectStdout() const {
            return true;
        }

        virtual void StartTesting() {}

        virtual void StartGroup( const std::string& groupName ) {
            if( groupName.empty() )
                m_statsForSuites.push_back( Stats( m_config.fullConfig()->name() ) );
            else
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
            m_currentStats->m_testCaseStats.push_back( TestCaseStats( testInfo.className, testInfo.name ) );
            m_currentTestCaseStats.push_back( &m_currentStats->m_testCaseStats.back() );
        }

        virtual void Result( const Catch::AssertionResult& assertionResult ) {
            if( assertionResult.getResultType() != ResultWas::Ok || m_config.fullConfig()->includeSuccessfulResults() ) {
                TestCaseStats& testCaseStats = m_currentStats->m_testCaseStats.back();
                TestStats stats;
                std::ostringstream oss;
                if( !assertionResult.getMessage().empty() )
                    oss << assertionResult.getMessage() << " at ";
                oss << assertionResult.getSourceInfo();
                stats.m_content = oss.str();
                stats.m_message = assertionResult.getExpandedExpression();
                stats.m_resultType = assertionResult.getTestMacroName();

                switch( assertionResult.getResultType() ) {
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
                    case ResultWas::DidntThrowException:
                        stats.m_element = "failure";
                        m_currentStats->m_failuresCount++;
                        break;
                    case ResultWas::Unknown:
                    case ResultWas::FailureBit:
                    case ResultWas::Exception:
                        stats.m_element = "* internal error *";
                        break;
                }
                testCaseStats.m_testStats.push_back( stats );
            }
        }

        virtual void EndTestCase( const Catch::TestCaseInfo&, const Totals&, const std::string& stdOut, const std::string& stdErr ) {
            m_currentTestCaseStats.pop_back();
            assert( m_currentTestCaseStats.empty() );
            TestCaseStats& testCaseStats = m_currentStats->m_testCaseStats.back();
            testCaseStats.m_stdOut = stdOut;
            testCaseStats.m_stdErr = stdErr;
            if( !stdOut.empty() )
                m_stdOut << stdOut << "\n";
            if( !stdErr.empty() )
                m_stdErr << stdErr << "\n";
        }

        virtual void Aborted() {
            // !TBD
        }

        virtual void EndTesting( const Totals& ) {
            XmlWriter xml( m_config.stream() );

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
                if( m_config.fullConfig()->showDurations() == ShowDurations::Never )
                    xml.writeAttribute( "time", it->m_timeInSeconds );
                else
                    xml.writeAttribute( "time", "" );
                xml.writeAttribute( "timestamp", "tbd" );

                OutputTestCases( xml, *it );
            }

            xml.scopedElement( "system-out" ).writeText( trim( m_stdOut.str() ), false );
            xml.scopedElement( "system-err" ).writeText( trim( m_stdErr.str() ), false );
        }

        void OutputTestCases( XmlWriter& xml, const Stats& stats ) {
            std::vector<TestCaseStats>::const_iterator it = stats.m_testCaseStats.begin();
            std::vector<TestCaseStats>::const_iterator itEnd = stats.m_testCaseStats.end();
            for(; it != itEnd; ++it ) {

                XmlWriter::ScopedElement e = xml.scopedElement( "testcase" );
                xml.writeAttribute( "classname", it->m_className );
                xml.writeAttribute( "name", it->m_name );
                if( m_config.fullConfig()->showDurations() == ShowDurations::Never )
                    xml.writeAttribute( "time", "" );
                else
                    xml.writeAttribute( "time", stats.m_timeInSeconds );

                OutputTestResult( xml, *it );

                std::string stdOut = trim( it->m_stdOut );
                if( !stdOut.empty() )
                    xml.scopedElement( "system-out" ).writeText( stdOut, false );
                std::string stdErr = trim( it->m_stdErr );
                if( !stdErr.empty() )
                    xml.scopedElement( "system-err" ).writeText( stdErr, false );
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

        Stats m_testSuiteStats;
        Stats* m_currentStats;
        std::vector<Stats> m_statsForSuites;
        std::vector<const TestCaseStats*> m_currentTestCaseStats;
        std::ostringstream m_stdOut;
        std::ostringstream m_stdErr;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED
