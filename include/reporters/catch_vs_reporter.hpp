/*
 *  Created by Malcolm on 4/11/2013.
 *  Copyright 2013 Malcolm Noyes. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_MSTEST_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_MSTEST_HPP_INCLUDED

#include "../internal/catch_interfaces_reporter.h"
//#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_text.h"
//#include "../internal/catch_console_colour.hpp"
#include "../internal/catch_version.h"
//#include <sstream>

namespace Catch {

#if (_MANAGED == 1) || (_M_CEE == 1) // detect CLR

    inline void write_output_message(const std::string& msg)
    {
        String^ tmp = gcnew String(msg.c_str());
        Console::WriteLine(tmp);
    }

#else // detect CLR

#ifdef _WINDLL

#ifdef _UNICODE
    inline void write_output_message(const std::string& msg)
    {
        std::wstringstream _s;
        _s << msg.c_str();
        std::wstring ws = _s.str();
        Logger::WriteMessage(ws.c_str());
    }
#else
    inline void write_output_message(const std::string& msg)
    {
        Logger::WriteMessage(msg.c_str());
    }
#endif

#endif // _WINDLL

#endif  // detect CLR

    struct MSTestReporter : SharedImpl<IStreamingReporter> {
        MSTestReporter( Ptr<IConfig> const& _fullConfig )
        :   m_config( _fullConfig ),
            m_headerPrinted( false ),
            m_atLeastOneTestCasePrinted( false )
        {}

        virtual ~MSTestReporter() {
            if( m_atLeastOneTestCasePrinted ) {
                write_output_message(stream.str());
            }
        }
    
        static std::string getDescription() {
            return "Reports test results to MSTest";
        }
        virtual ReporterPreferences getPreferences() const {
            ReporterPreferences prefs;
            prefs.shouldRedirectStdOut = true;
            return prefs;
        }

        //base
        virtual void noMatchingTestCases( std::string const& ) {}

        virtual void testRunStarting( TestRunInfo const& _testRunInfo ) {
            currentTestRunInfo = _testRunInfo;
        }
        virtual void testGroupStarting( GroupInfo const& _groupInfo ) {
            currentGroupInfo = _groupInfo;
        }

        virtual void testCaseStarting( TestCaseInfo const& _testInfo ) {
            currentTestCaseInfo = _testInfo;
        }
        virtual void sectionStarting( SectionInfo const& _sectionInfo ) {
            m_headerPrinted = false;
            m_sectionStack.push_back( _sectionInfo );
        }

        virtual void sectionEnded( SectionStats const& _sectionStats ) {
            if( _sectionStats.missingAssertions ) {
                lazyPrint();
                if( m_sectionStack.size() > 1 )
                    stream << "\r\n" << "No assertions in section";
                else
                    stream << "\r\n" << "No assertions in test case";
                stream << " '" << _sectionStats.sectionInfo.name << "'" << "\r\n" << "\r\n";
            }
            if( m_headerPrinted ) {
                if( m_config->showDurations() == ShowDurations::Always )
                    stream << "Completed in " << _sectionStats.durationInSeconds << "s" << "\r\n";
                m_headerPrinted = false;
            }
            else {
                if( m_config->showDurations() == ShowDurations::Always )
                    stream << _sectionStats.sectionInfo.name << " completed in " << _sectionStats.durationInSeconds << "s" << "\r\n";
            }
            m_sectionStack.pop_back();
        }
        virtual void testCaseEnded( TestCaseStats const& _testCaseStats ) {
            if( !_testCaseStats.stdOut.empty() ) {
                write_output_message(getDoubleDashes());
                write_output_message("Output to std::cout :");
                write_output_message(getDashes());
                write_output_message(_testCaseStats.stdOut);
                write_output_message(getDoubleDashes());
            }
            if( !_testCaseStats.stdErr.empty() ) {
                write_output_message(getDoubleDashes());
                write_output_message("Output to std::cerr :");
                write_output_message(getDashes());
                write_output_message(_testCaseStats.stdErr);
                write_output_message(getDoubleDashes());
            }
            if( _testCaseStats.totals.assertions.failed ) {
                Assert::IsTrue(false, L"At least one test failed - examine output for CHECK failures.");
            }
            m_headerPrinted = false;
            currentTestCaseInfo.reset();
            assert( m_sectionStack.empty() );
        }
        virtual void testGroupEnded( TestGroupStats const& _testGroupStats ) {
            if( currentGroupInfo.used ) {
                printSummaryDivider();
                stream << "Summary for group '" << _testGroupStats.groupInfo.name << "':" << "\r\n";
                printTotals( _testGroupStats.totals );
                stream << "\r\n" << "\r\n";
            }
            currentGroupInfo.reset();
        }
        virtual void testRunEnded( TestRunStats const& _testRunStats ) {
            if( m_atLeastOneTestCasePrinted )
                printTotalsDivider();
            printTotals( _testRunStats.totals );
            stream << "\r\n" << "\r\n";
            currentTestCaseInfo.reset();
            currentGroupInfo.reset();
            currentTestRunInfo.reset();
        }
        // base end

        virtual void assertionStarting( AssertionInfo const& ) {
        }

        virtual bool assertionEnded( AssertionStats const& _assertionStats ) {
            AssertionResult const& result = _assertionStats.assertionResult;

            // Drop out if result was successful and we're not printing those
            if( !m_config->includeSuccessfulResults() && result.isOk() )
                return false;

            lazyPrint();

            AssertionPrinter printer( stream, _assertionStats );
            printer.print();
            stream << "\r\n";
            return true;
        }

    private:

        class AssertionPrinter {
            void operator= ( AssertionPrinter const& );
        public:
            AssertionPrinter( std::ostream& _stream, AssertionStats const& _stats )
            :   stream( _stream ),
                stats( _stats ),
                result( _stats.assertionResult ),
                message( result.getMessage() ),
                messages( _stats.infoMessages )
            {
                switch( result.getResultType() ) {
                    case ResultWas::Ok:
                        passOrFail = "PASSED";
                        //if( result.hasMessage() )
                        if( _stats.infoMessages.size() == 1 )
                            messageLabel = "with message";
                        if( _stats.infoMessages.size() > 1 )
                            messageLabel = "with messages";
                        break;
                    case ResultWas::ExpressionFailed:
                        if( result.isOk() ) {
                            passOrFail = "FAILED - but was ok";
                        }
                        else {
                            passOrFail = "FAILED";
                        }
                        if( _stats.infoMessages.size() == 1 )
                            messageLabel = "with message";
                        if( _stats.infoMessages.size() > 1 )
                            messageLabel = "with messages";
                        break;
                    case ResultWas::ThrewException:
                        passOrFail = "FAILED";
                        messageLabel = "due to unexpected exception with message";
                        break;
                    case ResultWas::DidntThrowException:
                        passOrFail = "FAILED";
                        messageLabel = "because no exception was thrown where one was expected";
                        break;
                    case ResultWas::Info:
                        messageLabel = "info";
                        break;
                    case ResultWas::Warning:
                        messageLabel = "warning";
                        break;
                    case ResultWas::ExplicitFailure:
                        passOrFail = "FAILED";
                        if( _stats.infoMessages.size() == 1 )
                            messageLabel = "explicitly with message";
                        if( _stats.infoMessages.size() > 1 )
                            messageLabel = "explicitly with messages";
                        break;
                    // These cases are here to prevent compiler warnings
                    case ResultWas::Unknown:
                    case ResultWas::FailureBit:
                    case ResultWas::Exception:
                        passOrFail = "** internal error **";
                        break;
                }
            }

            void print() const {
                printSourceInfo();
                if( stats.totals.assertions.total() > 0 ) {
                    if( result.isOk() )
                        stream << "\r\n";
                    printResultType();
                    printOriginalExpression();
                    printReconstructedExpression();
                }
                else {
                    stream << "\r\n";
                }
                printMessage();
            }

        private:
            void printResultType() const {
                if( !passOrFail.empty() ) {
                    stream << passOrFail << ":" << "\r\n";
                }
            }
            void printOriginalExpression() const {
                if( result.hasExpression() ) {
                    stream  << "  ";
                    stream << result.getExpressionInMacro();
                    stream << "\r\n";
                }
            }
            void printReconstructedExpression() const {
                if( result.hasExpandedExpression() ) {
                    stream << "with expansion:" << "\r\n";
                    stream << Text( result.getExpandedExpression(), TextAttributes().setIndent(2) ) << "\r\n";
                }
            }
            void printMessage() const {
                if( !messageLabel.empty() )
                    stream << messageLabel << ":" << "\r\n";
                for( std::vector<MessageInfo>::const_iterator it = messages.begin(), itEnd = messages.end();
                        it != itEnd;
                        ++it ) {
                    stream << Text( it->message, TextAttributes().setIndent(2) ) << "\r\n";
                }
            }
            void printSourceInfo() const {
                stream << result.getSourceInfo() << ": ";
            }

            std::ostream& stream;
            AssertionStats const& stats;
            AssertionResult const& result;
            std::string passOrFail;
            std::string messageLabel;
            std::string message;
            std::vector<MessageInfo> messages;
        };

        void lazyPrint() {

            if( !currentTestRunInfo.used )
                lazyPrintRunInfo();
            if( !currentGroupInfo.used )
                lazyPrintGroupInfo();

            if( !m_headerPrinted ) {
                printTestCaseAndSectionHeader();
                m_headerPrinted = true;
            }
            m_atLeastOneTestCasePrinted = true;
        }
        void lazyPrintRunInfo() {
            stream  << getTildes() << "\r\n";
            stream  << "Using Catch v"  << libraryVersion::value.majorVersion << "."
                    << libraryVersion::value.minorVersion << " b"
                    << libraryVersion::value.buildNumber;
            if( libraryVersion::value.branchName != "master" )
                stream << " (" << libraryVersion::value.branchName << ")";
#if (_MANAGED == 1) || (_M_CEE == 1) // detect CLR
            stream  << " for a managed MSTest project." << "\r\n";
#else
#ifdef _WINDLL
            stream  << " for a native MSTest project." << "\r\n";
#endif
#endif

            currentTestRunInfo.used = true;
        }
        void lazyPrintGroupInfo() {
            if( currentGroupInfo.some() )
            {
                if( !currentGroupInfo->name.empty() && currentGroupInfo->groupsCounts > 1 ) {
                    printClosedHeader( "Group: " + currentGroupInfo->name );
                    currentGroupInfo.used = true;
                }
            }
        }
        void printTestCaseAndSectionHeader() {
            assert( !m_sectionStack.empty() );
            printOpenHeader( currentTestCaseInfo->name );

            if( m_sectionStack.size() > 1 ) {

                std::vector<SectionInfo>::const_iterator
                    it = m_sectionStack.begin()+1, // Skip first section (test case)
                    itEnd = m_sectionStack.end();
                for( ; it != itEnd; ++it )
                    printHeaderString( it->name, 2 );
            }

            SourceLineInfo lineInfo = m_sectionStack.front().lineInfo;

            if( !lineInfo.empty() ){
                stream << getDashes() << "\r\n";
                stream << lineInfo << "\r\n";
            }
            stream << getDots() << "\r\n" << "\r\n";
        }

        void printClosedHeader( std::string const& _name ) {
            printOpenHeader( _name );
            stream << getDots() << "\r\n";
        }
        void printOpenHeader( std::string const& _name ) {
            stream  << getDashes() << "\r\n";
            {
                printHeaderString( _name );
            }
        }

        // if string has a : in first line will set indent to follow it on
        // subsequent lines
        void printHeaderString( std::string const& _string, std::size_t indent = 0 ) {
            std::size_t i = _string.find( ": " );
            if( i != std::string::npos )
                i+=2;
            else
                i = 0;
            stream << Text( _string, TextAttributes()
                                        .setIndent( indent+i)
                                        .setInitialIndent( indent ) ) << "\r\n";
        }

        void printTotals( const Totals& totals ) {
            if( totals.assertions.total() == 0 ) {
                stream << "No tests ran";
            }
            else if( totals.assertions.failed ) {
                printCounts( "test case", totals.testCases );
                if( totals.testCases.failed > 0 ) {
                    stream << " (";
                    printCounts( "assertion", totals.assertions );
                    stream << ")";
                }
            }
            else {
                stream << "All tests passed ("
                        << pluralise( totals.assertions.passed, "assertion" ) << " in "
                        << pluralise( totals.testCases.passed, "test case" ) << ")";
            }
        }
        void printCounts( std::string const& label, Counts const& counts ) {
            if( counts.total() == 1 ) {
                stream << "1 " << label << " - ";
                if( counts.failed )
                    stream << "failed";
                else
                    stream << "passed";
            }
            else {
                stream << counts.total() << " " << label << "s ";
                if( counts.passed ) {
                    if( counts.failed )
                        stream << "- " << counts.failed << " failed";
                    else if( counts.passed == 2 )
                        stream << "- both passed";
                    else
                        stream << "- all passed";
                }
                else {
                    if( counts.failed == 2 )
                        stream << "- both failed";
                    else
                        stream << "- all failed";
                }
            }
        }

        void printTotalsDivider() {
            stream << getDoubleDashes() << "\r\n";
        }
        void printSummaryDivider() {
            stream << getDashes() << "\r\n";
        }
        static std::string getDashes() {
            std::string dashes( CATCH_CONFIG_CONSOLE_WIDTH-1, '-' );
            return dashes;
        }
        static std::string getDots() {
            std::string dots( CATCH_CONFIG_CONSOLE_WIDTH-1, '.' );
            return dots;
        }
        static std::string getDoubleDashes() {
            std::string doubleDashes( CATCH_CONFIG_CONSOLE_WIDTH-1, '=' );
            return doubleDashes;
        }
        static std::string getTildes() {
            std::string dots( CATCH_CONFIG_CONSOLE_WIDTH-1, '~' );
            return dots;
        }

    private:
        Ptr<IConfig> m_config;
        std::ostringstream stream;

        LazyStat<TestRunInfo> currentTestRunInfo;
        LazyStat<GroupInfo> currentGroupInfo;
        LazyStat<TestCaseInfo> currentTestCaseInfo;

        std::vector<SectionInfo> m_sectionStack;
        bool m_headerPrinted;
        bool m_atLeastOneTestCasePrinted;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_MSTEST_HPP_INCLUDED
