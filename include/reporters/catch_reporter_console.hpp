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
            m_printedCurrentSection( false ),
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

            AssertionPrinter printer( stream, _assertionStats );
            printer.print();
            stream << std::endl;
        }

        virtual void sectionStarting( SectionInfo const& _sectionInfo ) {
            m_printedCurrentSection = false;
            StreamingReporterBase::sectionStarting( _sectionInfo );
        }
        virtual void sectionEnded( SectionStats const& _sectionStats ) {
            if( _sectionStats.missingAssertions ) {
                lazyPrint();
                TextColour colour( TextColour::ResultError );
                stream << "\nNo assertions in section, '" << _sectionStats.sectionInfo.name << "'\n" << std::endl;
            }
            m_printedCurrentSection = false;
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
        
        class AssertionPrinter {
        public:
            AssertionPrinter( std::ostream& _stream, AssertionStats const& _stats )
            :   stream( _stream ),
                stats( _stats ),
                result( _stats.assertionResult ),
                colour( TextColour::None ),
                message( result.getMessage() )
            {
                switch( result.getResultType() ) {
                    case ResultWas::Ok:
                        colour = TextColour::Success;
                        passOrFail = "PASSED";
                        if( result.hasMessage() )
                            messageLabel = "with message";
                        break;
                    case ResultWas::ExpressionFailed:
                        if( result.isOk() ) {
                            colour = TextColour::Success;
                            passOrFail = "FAILED - but was ok";
                        }
                        else {
                            colour = TextColour::Error;
                            passOrFail = "FAILED";
                        }
                        if( result.hasMessage() ){
                            messageLabel = "with message";
                        }
                        break;
                    case ResultWas::ThrewException:
                        colour = TextColour::Error;
                        passOrFail = "FAILED";
                        messageLabel = "due to unexpected exception with message";
                        break;
                    case ResultWas::DidntThrowException:
                        colour = TextColour::Error;
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
                        colour = TextColour::Error;
                        messageLabel = "explicitly with message";
                        break;
                    case ResultWas::Exception:
                        passOrFail = "FAILED";
                        colour = TextColour::Error;
                        if( result.hasMessage() )
                            messageLabel = "with message";
                        break;
                        
                    // These cases are here to prevent compiler warnings
                    case ResultWas::Unknown:
                    case ResultWas::FailureBit:
                        passOrFail = "** internal error **";
                        colour = TextColour::Error;
                        break;
                }
            }
            
            void print() const {
                if( stats.totals.assertions.total() > 0 ) {
                    printResultType();
                    printOriginalExpression();
                    printReconstructedExpression();
                }
                printMessage();
                printSourceInfo();
            }
            
        private:
            void printResultType() const {
                if( !passOrFail.empty() ) {
                    TextColour colourGuard( colour );
                    stream << passOrFail << ":\n";
                }
            }
            void printOriginalExpression() const {
                if( result.hasExpression() ) {
                    TextColour colourGuard( TextColour::OriginalExpression );
                    stream  << "  ";
                    if( !result.getTestMacroName().empty() )
                        stream << result.getTestMacroName() << "( ";
                    stream << result.getExpression();
                    if( !result.getTestMacroName().empty() )
                        stream << " )";
                    stream << "\n";
                }
            }
            void printReconstructedExpression() const {
                if( result.hasExpandedExpression() ) {
                    stream << "with expansion:\n";
                    TextColour colourGuard( TextColour::ReconstructedExpression );
                    stream << wrapLongStrings( result.getExpandedExpression() ) << "\n";
                }
            }
            void printMessage() const {
                if( !messageLabel.empty() )
                    stream << messageLabel << ":" << "\n";
                if( !message.empty() )
                    stream << wrapLongStrings( message ) << "\n";
            }
            void printSourceInfo() const {
                TextColour colourGuard( TextColour::FileName );
                stream << result.getSourceInfo() << ":\n";
            }
            
            static std::string wrapLongStrings( std::string const& _string ){
                return Catch::wrapLongStrings( _string, CATCH_CONFIG_CONSOLE_WIDTH-1, 2 );
            }
            
            std::ostream& stream;
            AssertionStats const& stats;
            AssertionResult const& result;
            TextColour::Colours colour;
            std::string passOrFail;
            std::string messageLabel;
            std::string message;
        };
        
        void lazyPrint() {
            
            if( testRunInfo )
                lazyPrintRunInfo();
            if( unusedGroupInfo )
                lazyPrintGroupInfo();
            if( unusedTestCaseInfo )
                lazyPrintTestCaseInfo();
            if( currentSectionInfo && !m_printedCurrentSection )
                lazyPrintSectionInfo();
            
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
                printHeader( "Group: " + unusedGroupInfo->name );
                unusedGroupInfo.reset();
            }
        }
        void lazyPrintTestCaseInfo() {
            if( !currentSectionInfo ) {
                printHeader( unusedTestCaseInfo->name );
                stream << std::endl;
                unusedTestCaseInfo.reset();
            }
        }
        void lazyPrintSectionInfo() {
            
            std::vector<ThreadedSectionInfo*> sections;
            for(    ThreadedSectionInfo* section = currentSectionInfo.get();
                    section;
                    section = section->parent )
                sections.push_back( section );
            
            // Sections
            if( !sections.empty() ) {
                printHeader( unusedTestCaseInfo->name, false );
                
                typedef std::vector<ThreadedSectionInfo*>::const_reverse_iterator It;
                for( It it = sections.rbegin(), itEnd = sections.rend(); it != itEnd; ++it )
                    stream << "  " << (*it)->name << "\n";
                stream << getDots() << "\n" << std::endl;
            }
            m_printedCurrentSection = true;
        }

        void printHeader( std::string const& _name, bool closed = true ) {
            stream  << getDashes() << "\n"
                    << _name << "\n";
            if( closed )
                stream << getDots() << "\n";
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
            static const std::string dashes( CATCH_CONFIG_CONSOLE_WIDTH-1, '-' );
            return dashes;
        }
        static std::string const& getDots() {
            static const std::string dots( CATCH_CONFIG_CONSOLE_WIDTH-1, '.' );
            return dots;
        }
        static std::string const& getDoubleDashes() {
            static const std::string doubleDashes( CATCH_CONFIG_CONSOLE_WIDTH-1, '=' );
            return doubleDashes;
        }
        
    private:
        bool m_printedCurrentSection;
        bool m_atLeastOneTestCasePrinted;
    };

    INTERNAL_CATCH_REGISTER_REPORTER( "console", ConsoleReporter )

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
