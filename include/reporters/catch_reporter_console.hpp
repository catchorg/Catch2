/*
 *  Created by Phil on 5/12/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED

#include "catch_reporter_bases.hpp"

#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_console_colour.hpp"

namespace Catch {

    struct ConsoleReporter : StreamingReporterBase {
        ConsoleReporter( ReporterConfig const& _config )
        :   StreamingReporterBase( _config ),
            m_headerPrinted( false )
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

        virtual void noMatchingTestCases( std::string const& spec ) {
            stream << "No test cases matched '" << spec << "'" << std::endl;
        }

        virtual void assertionStarting( AssertionInfo const& ) {
        }

        virtual bool assertionEnded( AssertionStats const& _assertionStats ) {
            AssertionResult const& result = _assertionStats.assertionResult;

            bool printInfoMessages = true;

            // Drop out if result was successful and we're not printing those
            if( !m_config->includeSuccessfulResults() && result.isOk() ) {
                if( result.getResultType() != ResultWas::Warning )
                    return false;
                printInfoMessages = false;
            }

            lazyPrint();

            AssertionPrinter printer( stream, _assertionStats, printInfoMessages );
            printer.print();
            stream << std::endl;
            return true;
        }

        virtual void sectionStarting( SectionInfo const& _sectionInfo ) {
            m_headerPrinted = false;
            StreamingReporterBase::sectionStarting( _sectionInfo );
        }
        virtual void sectionEnded( SectionStats const& _sectionStats ) {
            if( _sectionStats.missingAssertions ) {
                lazyPrint();
                Colour colour( Colour::ResultError );
                if( m_sectionStack.size() > 1 )
                    stream << "\nNo assertions in section";
                else
                    stream << "\nNo assertions in test case";
                stream << " '" << _sectionStats.sectionInfo.name << "'\n" << std::endl;
            }
            if( m_headerPrinted ) {
                if( m_config->showDurations() == ShowDurations::Always )
                    stream << "Completed in " << _sectionStats.durationInSeconds << "s" << std::endl;
                m_headerPrinted = false;
            }
            else {
                if( m_config->showDurations() == ShowDurations::Always )
                    stream << _sectionStats.sectionInfo.name << " completed in " << _sectionStats.durationInSeconds << "s" << std::endl;
            }
            StreamingReporterBase::sectionEnded( _sectionStats );
        }

        virtual void testCaseEnded( TestCaseStats const& _testCaseStats ) {
            StreamingReporterBase::testCaseEnded( _testCaseStats );
            m_headerPrinted = false;
        }
        virtual void testGroupEnded( TestGroupStats const& _testGroupStats ) {
            if( currentGroupInfo.used ) {
                printSummaryDivider();
                stream << "Summary for group '" << _testGroupStats.groupInfo.name << "':\n";
                printTotals( _testGroupStats.totals );
                stream << "\n" << std::endl;
            }
            StreamingReporterBase::testGroupEnded( _testGroupStats );
        }
        virtual void testRunEnded( TestRunStats const& _testRunStats ) {
            printTotalsDivider( _testRunStats.totals );
            printTotals( _testRunStats.totals );
            stream << std::endl;
            StreamingReporterBase::testRunEnded( _testRunStats );
        }

    private:

        class AssertionPrinter {
            void operator= ( AssertionPrinter const& );
        public:
            AssertionPrinter( std::ostream& _stream, AssertionStats const& _stats, bool _printInfoMessages )
            :   stream( _stream ),
                stats( _stats ),
                result( _stats.assertionResult ),
                colour( Colour::None ),
                message( result.getMessage() ),
                messages( _stats.infoMessages ),
                printInfoMessages( _printInfoMessages )
            {
                switch( result.getResultType() ) {
                    case ResultWas::Ok:
                        colour = Colour::Success;
                        passOrFail = "PASSED";
                        //if( result.hasMessage() )
                        if( _stats.infoMessages.size() == 1 )
                            messageLabel = "with message";
                        if( _stats.infoMessages.size() > 1 )
                            messageLabel = "with messages";
                        break;
                    case ResultWas::ExpressionFailed:
                        if( result.isOk() ) {
                            colour = Colour::Success;
                            passOrFail = "FAILED - but was ok";
                        }
                        else {
                            colour = Colour::Error;
                            passOrFail = "FAILED";
                        }
                        if( _stats.infoMessages.size() == 1 )
                            messageLabel = "with message";
                        if( _stats.infoMessages.size() > 1 )
                            messageLabel = "with messages";
                        break;
                    case ResultWas::ThrewException:
                        colour = Colour::Error;
                        passOrFail = "FAILED";
                        messageLabel = "due to unexpected exception with message";
                        break;
                    case ResultWas::FatalErrorCondition:
                        colour = Colour::Error;
                        passOrFail = "FAILED";
                        messageLabel = "due to a fatal error condition";
                        break;
                    case ResultWas::DidntThrowException:
                        colour = Colour::Error;
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
                        colour = Colour::Error;
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
                        colour = Colour::Error;
                        break;
                }
            }

            void print() const {
                printSourceInfo();
                if( stats.totals.assertions.total() > 0 ) {
                    if( result.isOk() )
                        stream << "\n";
                    printResultType();
                    printOriginalExpression();
                    printReconstructedExpression();
                }
                else {
                    stream << "\n";
                }
                printMessage();
            }

        private:
            void printResultType() const {
                if( !passOrFail.empty() ) {
                    Colour colourGuard( colour );
                    stream << passOrFail << ":\n";
                }
            }
            void printOriginalExpression() const {
                if( result.hasExpression() ) {
                    Colour colourGuard( Colour::OriginalExpression );
                    stream  << "  ";
                    stream << result.getExpressionInMacro();
                    stream << "\n";
                }
            }
            void printReconstructedExpression() const {
                if( result.hasExpandedExpression() ) {
                    stream << "with expansion:\n";
                    Colour colourGuard( Colour::ReconstructedExpression );
                    stream << Text( result.getExpandedExpression(), TextAttributes().setIndent(2) ) << "\n";
                }
            }
            void printMessage() const {
                if( !messageLabel.empty() )
                    stream << messageLabel << ":" << "\n";
                for( std::vector<MessageInfo>::const_iterator it = messages.begin(), itEnd = messages.end();
                        it != itEnd;
                        ++it ) {
                    // If this assertion is a warning ignore any INFO messages
                    if( printInfoMessages || it->type != ResultWas::Info )
                        stream << Text( it->message, TextAttributes().setIndent(2) ) << "\n";
                }
            }
            void printSourceInfo() const {
                Colour colourGuard( Colour::FileName );
                stream << result.getSourceInfo() << ": ";
            }

            std::ostream& stream;
            AssertionStats const& stats;
            AssertionResult const& result;
            Colour::Code colour;
            std::string passOrFail;
            std::string messageLabel;
            std::string message;
            std::vector<MessageInfo> messages;
            bool printInfoMessages;
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
        }
        void lazyPrintRunInfo() {
            stream  << "\n" << getLineOfChars<'~'>() << "\n";
            Colour colour( Colour::SecondaryText );
            stream  << currentTestRunInfo->name
                    << " is a Catch v"  << libraryVersion << " host application.\n"
                    << "Run with -? for options\n\n";

            if( m_config->rngSeed() != 0 )
                stream << "Randomness seeded to: " << m_config->rngSeed() << "\n\n";

            currentTestRunInfo.used = true;
        }
        void lazyPrintGroupInfo() {
            if( !currentGroupInfo->name.empty() && currentGroupInfo->groupsCounts > 1 ) {
                printClosedHeader( "Group: " + currentGroupInfo->name );
                currentGroupInfo.used = true;
            }
        }
        void printTestCaseAndSectionHeader() {
            assert( !m_sectionStack.empty() );
            printOpenHeader( currentTestCaseInfo->name );

            if( m_sectionStack.size() > 1 ) {
                Colour colourGuard( Colour::Headers );

                std::vector<SectionInfo>::const_iterator
                    it = m_sectionStack.begin()+1, // Skip first section (test case)
                    itEnd = m_sectionStack.end();
                for( ; it != itEnd; ++it )
                    printHeaderString( it->name, 2 );
            }

            SourceLineInfo lineInfo = m_sectionStack.front().lineInfo;

            if( !lineInfo.empty() ){
                stream << getLineOfChars<'-'>() << "\n";
                Colour colourGuard( Colour::FileName );
                stream << lineInfo << "\n";
            }
            stream << getLineOfChars<'.'>() << "\n" << std::endl;
        }

        void printClosedHeader( std::string const& _name ) {
            printOpenHeader( _name );
            stream << getLineOfChars<'.'>() << "\n";
        }
        void printOpenHeader( std::string const& _name ) {
            stream  << getLineOfChars<'-'>() << "\n";
            {
                Colour colourGuard( Colour::Headers );
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
                                        .setInitialIndent( indent ) ) << "\n";
        }

        struct SummaryColumn {

            SummaryColumn( std::string const& _label, Colour::Code _colour )
            :   label( _label ),
                colour( _colour )
            {}
            SummaryColumn addRow( std::size_t count ) {
                std::ostringstream oss;
                oss << count;
                std::string row = oss.str();
                for( std::vector<std::string>::iterator it = rows.begin(); it != rows.end(); ++it ) {
                    while( it->size() < row.size() )
                        *it = " " + *it;
                    while( it->size() > row.size() )
                        row = " " + row;
                }
                rows.push_back( row );
                return *this;
            }

            std::string label;
            Colour::Code colour;
            std::vector<std::string> rows;

        };

        void printTotals( Totals const& totals ) {
            if( totals.testCases.total() == 0 ) {
                stream << Colour( Colour::Warning ) << "No tests ran\n";
            }
            else if( totals.assertions.total() > 0 && totals.assertions.allPassed() ) {
                stream << Colour( Colour::ResultSuccess ) << "All tests passed";
                stream << " ("
                        << pluralise( totals.assertions.passed, "assertion" ) << " in "
                        << pluralise( totals.testCases.passed, "test case" ) << ")"
                        << "\n";
            }
            else {

                std::vector<SummaryColumn> columns;
                columns.push_back( SummaryColumn( "", Colour::None )
                                        .addRow( totals.testCases.total() )
                                        .addRow( totals.assertions.total() ) );
                columns.push_back( SummaryColumn( "passed", Colour::Success )
                                        .addRow( totals.testCases.passed )
                                        .addRow( totals.assertions.passed ) );
                columns.push_back( SummaryColumn( "failed", Colour::ResultError )
                                        .addRow( totals.testCases.failed )
                                        .addRow( totals.assertions.failed ) );
                columns.push_back( SummaryColumn( "failed as expected", Colour::ResultExpectedFailure )
                                        .addRow( totals.testCases.failedButOk )
                                        .addRow( totals.assertions.failedButOk ) );

                printSummaryRow( "test cases", columns, 0 );
                printSummaryRow( "assertions", columns, 1 );
            }
        }
        void printSummaryRow( std::string const& label, std::vector<SummaryColumn> const& cols, std::size_t row ) {
            for( std::vector<SummaryColumn>::const_iterator it = cols.begin(); it != cols.end(); ++it ) {
                std::string value = it->rows[row];
                if( it->label.empty() ) {
                    stream << label << ": ";
                    if( value != "0" )
                        stream << value;
                    else
                        stream << Colour( Colour::Warning ) << "- none -";
                }
                else if( value != "0" ) {
                    stream  << Colour( Colour::LightGrey ) << " | ";
                    stream  << Colour( it->colour )
                            << value << " " << it->label;
                }
            }
            stream << "\n";
        }

        static std::size_t makeRatio( std::size_t number, std::size_t total ) {
            std::size_t ratio = total > 0 ? CATCH_CONFIG_CONSOLE_WIDTH * number/ total : 0;
            return ( ratio == 0 && number > 0 ) ? 1 : ratio;
        }
        static std::size_t& findMax( std::size_t& i, std::size_t& j, std::size_t& k ) {
            if( i > j && i > k )
                return i;
            else if( j > k )
                return j;
            else
                return k;
        }

        void printTotalsDivider( Totals const& totals ) {
            if( totals.testCases.total() > 0 ) {
                std::size_t failedRatio = makeRatio( totals.testCases.failed, totals.testCases.total() );
                std::size_t failedButOkRatio = makeRatio( totals.testCases.failedButOk, totals.testCases.total() );
                std::size_t passedRatio = makeRatio( totals.testCases.passed, totals.testCases.total() );
                while( failedRatio + failedButOkRatio + passedRatio < CATCH_CONFIG_CONSOLE_WIDTH-1 )
                    findMax( failedRatio, failedButOkRatio, passedRatio )++;
                while( failedRatio + failedButOkRatio + passedRatio > CATCH_CONFIG_CONSOLE_WIDTH-1 )
                    findMax( failedRatio, failedButOkRatio, passedRatio )--;

                stream << Colour( Colour::Error ) << std::string( failedRatio, '=' );
                stream << Colour( Colour::ResultExpectedFailure ) << std::string( failedButOkRatio, '=' );
                if( totals.testCases.allPassed() )
                    stream << Colour( Colour::ResultSuccess ) << std::string( passedRatio, '=' );
                else
                    stream << Colour( Colour::Success ) << std::string( passedRatio, '=' );
            }
            else {
                stream << Colour( Colour::Warning ) << std::string( CATCH_CONFIG_CONSOLE_WIDTH-1, '=' );
            }
            stream << "\n";
        }
        void printSummaryDivider() {
            stream << getLineOfChars<'-'>() << "\n";
        }

    private:
        bool m_headerPrinted;
    };

    INTERNAL_CATCH_REGISTER_REPORTER( "console", ConsoleReporter )

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
