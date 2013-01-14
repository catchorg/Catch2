/*
 *  Created by Phil on 5/12/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED

#include "../internal/catch_interfaces_reporter.h"
#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_console_colour.hpp"

namespace Catch {

    struct ConsoleReporter : StreamingReporterBase {
        ConsoleReporter( ReporterConfig const& _config )
        :   StreamingReporterBase( _config ),
            m_atLeastOneTestCasePrinted( false )
        {}

        virtual ~ConsoleReporter();
        static std::string getDescription() {
            return "Reports test results as plain lines of text";
        }
        virtual ReporterPreferences getPreferences() const {
            ReporterPreferences prefs;
            prefs.shouldRedirectStdOut = false;
            return prefs;

        }
        
        virtual void assertionStarting( AssertionInfo const& ) {
        }

        virtual void assertionEnded( AssertionStats const& _assertionStats ) {
            AssertionResult const& result = _assertionStats.assertionResult;
            
            // Drop out if result was successful and we're not printing those
            if( !m_config.includeSuccessfulResults() && result.isOk() )
                return;
            
            lazyPrint();

            {
                TextColour colour( TextColour::FileName );
                stream << result.getSourceInfo() << ":\n";
            }
            
            bool endsWithNewLine = false;
            if( _assertionStats.totals.assertions.total() > 0 ) {
                endsWithNewLine = printOriginalExpression( result );
                endsWithNewLine = printResultType( result );
                endsWithNewLine |= printReconstructedExpression( result );
            }
            endsWithNewLine |= printMessage( result );
            if( !endsWithNewLine )
                stream << "\n";
            stream << std::endl;
        }
        
        bool printResultType( AssertionResult const& _result ) {
            if( _result.getResultType() == ResultWas::Info ||
                _result.getResultType() == ResultWas::Warning ) {
            }
            else if( _result.succeeded() ) {
                TextColour successColour( TextColour::Success );
                stream << "passed ";
            }
            else if( _result.isOk() ) {
                TextColour okAnywayColour( TextColour::Success );
                stream << "failed - but was ok ";
            }
            else {
                TextColour errorColour( TextColour::Error );
                stream << "failed ";
            }
            return false;
        }
        bool printOriginalExpression( AssertionResult const& _result ) {
            if( _result.hasExpression() ) {
                TextColour colour( TextColour::OriginalExpression );
                stream  << "  ";
                if( !_result.getTestMacroName().empty() )
                    stream << _result.getTestMacroName() << "( ";
                stream << _result.getExpression();
                if( !_result.getTestMacroName().empty() )
                    stream << " )";
                stream << "\n";
                return true;
            }
            return false;
        }
        bool printReconstructedExpression( AssertionResult const& _result ) {
            if( _result.hasExpandedExpression() ) {
                stream << "with expansion:\n";
                TextColour colour( TextColour::ReconstructedExpression );
                stream << wrapLongStrings( _result.getExpandedExpression() ) << "\n";
                return true;
            }
            return false;
        }        
        bool printMessage( AssertionResult const& _result ) {
            std::pair<std::string, std::string> message = getMessage( _result );
            bool endsWithNewLine = false;
            if( !message.first.empty() ) {
                stream << message.first << ":" << "\n";
                endsWithNewLine = true;
            }
            if( !message.second.empty() ) {
                stream << wrapLongStrings( message.second ) << "\n";
                endsWithNewLine = true;
            }
            return endsWithNewLine;
        }
        std::pair<std::string, std::string> getMessage( AssertionResult const& _result ) {
            switch( _result.getResultType() ) {
                case ResultWas::ThrewException:
                    return std::make_pair( "due to unexpected exception with message", _result.getMessage() );
                case ResultWas::DidntThrowException:
                    return std::make_pair( "because no exception was thrown where one was expected", "" );
                case ResultWas::Info:
                    return std::make_pair( "info", _result.getMessage() );
                case ResultWas::Warning:
                    return std::make_pair( "warning", _result.getMessage() );
                case ResultWas::ExplicitFailure:
                    return std::make_pair( "explicitly with message", _result.getMessage() );

                case ResultWas::Unknown: // These cases are here to prevent compiler warnings
                case ResultWas::Ok:
                case ResultWas::FailureBit:
                case ResultWas::ExpressionFailed:
                case ResultWas::Exception:
                    if( _result.hasMessage() )
                        return std::make_pair( "with message", _result.getMessage() );
                    else
                        return std::make_pair( "", "" );
            }
        }
                
        virtual void sectionEnded( SectionStats const& _sectionStats ) {
            if( _sectionStats.missingAssertions ) {
                lazyPrint();
                TextColour colour( TextColour::ResultError );
                stream << "\nNo assertions in section, '" << _sectionStats.sectionInfo.name << "'\n" << std::endl;
            }
            StreamingReporterBase::sectionEnded( _sectionStats );
        }

        virtual void testCaseEnded( TestCaseStats const& _testCaseStats ) {

            if( _testCaseStats.missingAssertions ) {
                lazyPrint();
                TextColour colour( TextColour::ResultError );
                stream << "\nNo assertions in test case, '" << _testCaseStats.testInfo.name << "'\n" << std::endl;
            }
            StreamingReporterBase::testCaseEnded( _testCaseStats );
        }
        virtual void testGroupEnded( TestGroupStats const& _testGroupStats ) {
            if( !unusedGroupInfo ) {
                printSummaryDivider();
                stream << "Summary for group '" << _testGroupStats.groupInfo.name << "':\n";
                printTotals( _testGroupStats.totals );
                stream << "\n" << std::endl;
            }
            StreamingReporterBase::testGroupEnded( _testGroupStats );
        }
        virtual void testRunEnded( TestRunStats const& _testRunStats ) {
            if( m_atLeastOneTestCasePrinted )
                printTotalsDivider();
            printTotals( _testRunStats.totals );
            stream << "\n" << std::endl;
            StreamingReporterBase::testRunEnded( _testRunStats );
        }

    private:
        std::string wrapLongStrings( std::string const& _string ) {
            return Catch::wrapLongStrings( _string, 70, 2 );
        }
        
        void lazyPrint() {
            
            bool needsNewline = unusedTestCaseInfo || ( currentSectionInfo && !currentSectionInfo->printed );
            
            if( testRunInfo )
                lazyPrintRunInfo();
            if( unusedGroupInfo )
                lazyPrintGroupInfo();
            if( unusedTestCaseInfo )
                lazyPrintTestCaseInfo();
            if( currentSectionInfo && !currentSectionInfo->printed )
                lazyPrintSectionInfo();
            if( needsNewline )
                stream << "\n";
            
            m_atLeastOneTestCasePrinted = true;
        }
        void lazyPrintRunInfo() {
            stream  << "\n"  << testRunInfo->name
                    << " is a CATCH v"  << libraryVersion.majorVersion << "."
                    << libraryVersion.minorVersion << " b"
                    << libraryVersion.buildNumber;
            if( libraryVersion.branchName != "master" )
                stream << " (" << libraryVersion.branchName << ")";
            stream  << " host application.\n"
                    << "Run with -? for options\n\n";
            
            testRunInfo.reset();
        }
        void lazyPrintGroupInfo() {
            if( !unusedGroupInfo->name.empty() && unusedGroupInfo->groupsCounts > 1 ) {
                printHeader( "Group", unusedGroupInfo->name );
                unusedGroupInfo.reset();
            }
        }
        void lazyPrintTestCaseInfo() {
            printHeader( "Test case", unusedTestCaseInfo->name );
            unusedTestCaseInfo.reset();
        }
        void lazyPrintSectionInfo() {
            std::vector<ThreadedSectionInfo*> sections;
            for(    ThreadedSectionInfo* section = unusedSectionInfo.get();
                    section && !section->printed;
                    section = section->parent.get() )
                sections.push_back( section );
            
            typedef std::vector<ThreadedSectionInfo*>::const_reverse_iterator It;
            for( It it = sections.rbegin(), itEnd = sections.rend(); it != itEnd; ++it ) {
                printHeader( "Section", (*it)->name );
                (*it)->printed = true;
            }
            unusedSectionInfo.reset();
        }
        
        void printHeader( std::string const& _type, std::string const& _name ) {
            stream  << getDashes() << "\n"
                    << _type << ": '" << _name << "'\n"
                    << getDashes() << std::endl;
        }
        void printTotals( const Totals& totals ) {
            if( totals.assertions.total() == 0 ) {
                stream << "No tests ran";
            }
            else if( totals.assertions.failed ) {
                TextColour colour( TextColour::ResultError );
                printCounts( "test case", totals.testCases );
                if( totals.testCases.failed > 0 ) {
                    stream << " (";
                    printCounts( "assertion", totals.assertions );
                    stream << ")";
                }
            }
            else {
                TextColour colour( TextColour::ResultSuccess );
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
            stream << getDoubleDashes() << "\n";
        }
        void printSummaryDivider() {
            stream << getDashes() << "\n";
        }
        static std::string const& getDashes() {
            static const std::string dashes
                = "----------------------------------------------------------------";
            return dashes;
        }
        static std::string const& getDoubleDashes() {
            static const std::string doubleDashes
                = "================================================================";
            return doubleDashes;
        }
        
    private:
        bool m_atLeastOneTestCasePrinted;

    };

    INTERNAL_CATCH_REGISTER_REPORTER( "console", ConsoleReporter )

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
