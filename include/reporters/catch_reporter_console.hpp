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
        void lazyPrintRunInfo() {
            printHeader( "Started testing", testRunInfo->name );
            testRunInfo.reset();
        }
        void lazyPrintGroupInfo() {
            if( !unusedGroupInfo->name.empty() )
                printHeader( "Group", unusedGroupInfo->name );
            unusedGroupInfo.reset();
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

        static std::string const& getDashes() {
            static const std::string dashes = "----------------------------------------------------------------";
            return dashes;
        }
        static std::string const& getDoubleDashes() {
            static const std::string doubleDashes = "================================================================";
            return doubleDashes;
        }
        static std::string const& getSpaces() {
            static const std::string spaces = "                                                               ";
            return spaces;
        }
        static std::string getSpaces( int spaces ) {
            return getSpaces().substr( 0, spaces > 0 ? static_cast<std::size_t>( spaces ) : 0 );
        }
        
        void printHeader( std::string const& _type, std::string const& _name ) {
            std::size_t labelLen =  _type.size() + _name.size() + 8;
            std::size_t dashLen = getDashes().size();
            stream  << "-- " << _type << ": '" << _name << "' "
                    << getDashes().substr( 0, labelLen < dashLen ? dashLen - labelLen : 0 )
                    << std::endl;
        }

        void lazyPrint() {
            if( testRunInfo )
                lazyPrintRunInfo();
            if( unusedGroupInfo )
                lazyPrintGroupInfo();
            if( unusedTestCaseInfo )
                lazyPrintTestCaseInfo();
            if( currentSectionInfo && !currentSectionInfo->printed )
                lazyPrintSectionInfo();
        }

        virtual void assertionStarting( AssertionInfo const& ) {
        }
        virtual void assertionEnded( AssertionStats const& _assertionStats ) {

            AssertionResult const& result = _assertionStats.assertionResult;
            
            // Drop out if result was successful and we're not printing those
            if( !m_config.includeSuccessfulResults() && result.isOk() )
                return;

            lazyPrint();

            int inset = printLineInfo( result.getSourceInfo() );

            if( result.hasExpression() ) {
                TextColour colour( TextColour::OriginalExpression );
                stream << result.getExpression() << "\n";
                if( result.succeeded() ) {
                    TextColour successColour( TextColour::Success );
                    stream << "succeeded";
                }
                else {
                    TextColour errorColour( TextColour::Error );
                    stream << "failed";
                    if( result.isOk() ) {
                        TextColour okAnywayColour( TextColour::Success );
                        stream << " - but was ok";
                    }
                }
            }

            switch( result.getResultType() ) {
                case ResultWas::ThrewException:
                {
                    TextColour colour( TextColour::Error );
                    if( result.hasExpression() )
                        stream << " with unexpected";
                    else
                        stream << "Unexpected";
                    stream << " exception with message: '" << result.getMessage() << "'";
                }
                    break;
                case ResultWas::DidntThrowException:
                {
                    TextColour colour( TextColour::Error );
                    if( result.hasExpression() )
                        stream << " because no exception was thrown where one was expected";
                    else
                        stream << "No exception thrown where one was expected";
                }
                    break;
                case ResultWas::Info:
                {
                    TextColour colour( TextColour::ReconstructedExpression );
                    streamVariableLengthText( "info", result.getMessage() );
                }
                    break;
                case ResultWas::Warning:
                {
                    TextColour colour( TextColour::ReconstructedExpression );
                    streamVariableLengthText( "warning", result.getMessage() );
                }
                    break;
                case ResultWas::ExplicitFailure:
                {
                    TextColour colour( TextColour::Error );
                    stream << "failed with message: '" << result.getMessage() << "'";
                }
                    break;
                case ResultWas::Unknown: // These cases are here to prevent compiler warnings
                case ResultWas::Ok:
                case ResultWas::FailureBit:
                case ResultWas::ExpressionFailed:
                case ResultWas::Exception:
                    if( !result.hasExpression() ) {
                        if( result.succeeded() ) {
                            TextColour colour( TextColour::Success );
                            stream << " succeeded";
                        }
                        else {
                            TextColour colour( TextColour::Error );
                            stream << " failed";
                            if( result.isOk() ) {
                                TextColour okAnywayColour( TextColour::Success );
                                stream << " - but was ok";
                            }
                        }
                    }
                    if( result.hasMessage() ) {
                        stream << "\n";
                        TextColour colour( TextColour::ReconstructedExpression );
                        streamVariableLengthText( "with message", result.getMessage() );
                    }
                    break;
            }

            if( result.hasExpandedExpression() ) {
                stream << "\nfor: ";
                TextColour colour( TextColour::ReconstructedExpression );
                stream << getSpaces( inset-5 ) << result.getExpandedExpression();
            }
            
            stream << "\n" << std::endl;
        }
        
        void streamVariableLengthText( std::string const& prefix, std::string const& text ) {
            std::string trimmed = trim( text );
            if( trimmed.find_first_of( "\r\n" ) == std::string::npos ) {
                stream << "[" << prefix << ": " << trimmed << "]";
            }
            else {
                stream  << "\n[" << prefix << "] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << trimmed
                        << "\n[end of " << prefix << "] <<<<<<<<<<<<<<<<<<<<<<<<\n";
            }
        }
        
        void printAssertionCounts( std::string const& label, Counts const& counts ) {
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

        void printTotals( const Totals& totals ) {
            if( totals.assertions.total() == 0 ) {
                stream << "No tests ran";
            }
            else if( totals.assertions.failed ) {
                TextColour colour( TextColour::ResultError );
                printAssertionCounts( "test case", totals.testCases );
                if( totals.testCases.failed > 0 ) {
                    stream << " (";
                    printAssertionCounts( "assertion", totals.assertions );
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

        virtual void sectionEnded( SectionStats const& _sectionStats ) {
            resetLastPrintedLine();
            if( _sectionStats.missingAssertions ) {
                lazyPrint();
                TextColour colour( TextColour::ResultError );
                stream << "\nNo assertions in section, '" << _sectionStats.sectionInfo.name << "'\n" << std::endl;
            }
            if( currentSectionInfo && currentSectionInfo->printed ) {
                printSummarDivider();
                stream << "Summary for section '" << _sectionStats.sectionInfo.name << "':\n";
                Counts const& assertions = _sectionStats.assertions;
                if( assertions.failed ) {
                    TextColour colour( TextColour::ResultError );
                    printAssertionCounts( "assertion", assertions );
                }
                else {
                    TextColour colour( TextColour::ResultSuccess );
                    stream  << ( assertions.passed > 1 ? "All " : "" )
                            << pluralise( assertions.passed, "assertion" ) << " passed" ;
                }
                stream << "\n" << std::endl;
            }
            StreamingReporterBase::sectionEnded( _sectionStats );
        }
        virtual void testCaseEnded( TestCaseStats const& _testCaseStats ) {
            resetLastPrintedLine();
            if( _testCaseStats.missingAssertions ) {
                lazyPrint();
                TextColour colour( TextColour::ResultError );
                stream << "\nNo assertions in test case, '" << _testCaseStats.testInfo.name << "'\n" << std::endl;
            }
            if( !unusedTestCaseInfo ) {
                m_atLeastOneTestCasePrinted = true;
                printSummarDivider();
                stream << "Summary for test case '" << _testCaseStats.testInfo.name << "':\n";
                printTotals( _testCaseStats.totals );
                stream << "\n" << std::endl;
            }
            StreamingReporterBase::testCaseEnded( _testCaseStats );
        }
        virtual void testGroupEnded( TestGroupStats const& _testGroupStats ) {
            if( !unusedGroupInfo ) {
                printSummarDivider();
                stream << "Summary for group '" << _testGroupStats.groupInfo.name << "':\n";
                printTotals( _testGroupStats.totals );
                stream << "\n" << std::endl;
            }
            StreamingReporterBase::testGroupEnded( _testGroupStats );
        }
        virtual void testRunEnded( TestRunStats const& _testRunStats ) {
            if( m_atLeastOneTestCasePrinted )
                printTotalsDivider();
            stream << "Summary for all tests in '" << _testRunStats.runInfo.name << "':\n";
            printTotals( _testRunStats.totals );
            stream << "\n" << std::endl;
            StreamingReporterBase::testRunEnded( _testRunStats );
        }

    private:
        void printTotalsDivider() {
            stream << "================================================================\n";
        }
        void printSummarDivider() {
            stream << "----------------------------------------------------------------\n";
        }
        static int countDigits( std::size_t number ) {
            int digits = 1;
            for( ; number != 0; digits++, number /= 10 );
            return digits;
        }

        // Returns number of characters printed
        int printLineInfo( SourceLineInfo const& lineInfo ) {
            if( lineInfo.empty() )
                return 0;
            if( m_lastPrintedLine.empty() ||
                    m_lastPrintedLine.file != lineInfo.file ||
                    abs( static_cast<int>( m_lastPrintedLine.line ) - static_cast<int>( lineInfo.line ) ) > 20 ) {
                TextColour colour( TextColour::FileName );
                stream << lineInfo << "\n";
            }
            TextColour colour( TextColour::FileName );
            stream << "[" << lineInfo.line << "]  ";
            m_lastPrintedLine = lineInfo;
            return 3 + countDigits( lineInfo.line );
        }
        void resetLastPrintedLine() {
            m_lastPrintedLine = SourceLineInfo();
        }
        bool m_atLeastOneTestCasePrinted;
        SourceLineInfo m_lastPrintedLine;

    };

    INTERNAL_CATCH_REGISTER_REPORTER( "console", ConsoleReporter )

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
