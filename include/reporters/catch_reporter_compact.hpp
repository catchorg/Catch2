/*
 *  Created by Martin Moene on 2013-12-05.
 *  Copyright 2012 Martin Moene. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_COMPACT_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_COMPACT_HPP_INCLUDED

#include "catch_reporter_bases.hpp"

#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_console_colour.hpp"

namespace Catch {

    struct CompactReporter : StreamingReporterBase {

        CompactReporter( ReporterConfig const& _config )
        : StreamingReporterBase( _config )
        {}

        virtual ~CompactReporter();

        static std::string getDescription() {
            return "Reports test results on a single line, suitable for IDEs";
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

            AssertionPrinter printer( stream, _assertionStats, printInfoMessages );
            printer.print();

            stream << std::endl;
            return true;
        }

        virtual void testRunEnded( TestRunStats const& _testRunStats ) {
            printTotals( _testRunStats.totals );
            stream << "\n" << std::endl;
            StreamingReporterBase::testRunEnded( _testRunStats );
        }

    private:
        class AssertionPrinter {
            void operator= ( AssertionPrinter const& );
        public:
            AssertionPrinter( std::ostream& _stream, AssertionStats const& _stats, bool _printInfoMessages )
            : stream( _stream )
            , stats( _stats )
            , result( _stats.assertionResult )
            , messages( _stats.infoMessages )
            , itMessage( _stats.infoMessages.begin() )
            , printInfoMessages( _printInfoMessages )
            {}

            void print() {
                printSourceInfo();

                itMessage = messages.begin();

                switch( result.getResultType() ) {
                    case ResultWas::Ok:
                        printResultType( Colour::ResultSuccess, passedString() );
                        printOriginalExpression();
                        printReconstructedExpression();
                        if ( ! result.hasExpression() )
                            printRemainingMessages( Colour::None );
                        else
                            printRemainingMessages();
                        break;
                    case ResultWas::ExpressionFailed:
                        if( result.isOk() )
                            printResultType( Colour::ResultSuccess, failedString() + std::string( " - but was ok" ) );
                        else
                            printResultType( Colour::Error, failedString() );
                        printOriginalExpression();
                        printReconstructedExpression();
                        printRemainingMessages();
                        break;
                    case ResultWas::ThrewException:
                        printResultType( Colour::Error, failedString() );
                        printIssue( "unexpected exception with message:" );
                        printMessage();
                        printExpressionWas();
                        printRemainingMessages();
                        break;
                    case ResultWas::DidntThrowException:
                        printResultType( Colour::Error, failedString() );
                        printIssue( "expected exception, got none" );
                        printExpressionWas();
                        printRemainingMessages();
                        break;
                    case ResultWas::Info:
                        printResultType( Colour::None, "info" );
                        printMessage();
                        printRemainingMessages();
                        break;
                    case ResultWas::Warning:
                        printResultType( Colour::None, "warning" );
                        printMessage();
                        printRemainingMessages();
                        break;
                    case ResultWas::ExplicitFailure:
                        printResultType( Colour::Error, failedString() );
                        printIssue( "explicitly" );
                        printRemainingMessages( Colour::None );
                        break;
                    // These cases are here to prevent compiler warnings
                    case ResultWas::Unknown:
                    case ResultWas::FailureBit:
                    case ResultWas::Exception:
                        printResultType( Colour::Error, "** internal error **" );
                        break;
                }
            }

        private:
            // Colour::LightGrey

            static Colour::Code dimColour() { return Colour::FileName; }

#ifdef CATCH_PLATFORM_MAC
            static const char* failedString() { return "FAILED"; }
            static const char* passedString() { return "PASSED"; }
#else
            static const char* failedString() { return "failed"; }
            static const char* passedString() { return "passed"; }
#endif

            void printSourceInfo() const {
                Colour colourGuard( Colour::FileName );
                stream << result.getSourceInfo() << ":";
            }

            void printResultType( Colour::Code colour, std::string passOrFail ) const {
                if( !passOrFail.empty() ) {
                    {
                        Colour colourGuard( colour );
                        stream << " " << passOrFail;
                    }
                    stream << ":";
                }
            }

            void printIssue( std::string issue ) const {
                stream << " " << issue;
            }

            void printExpressionWas() {
                if( result.hasExpression() ) {
                    stream << ";";
                    {
                        Colour colour( dimColour() );
                        stream << " expression was:";
                    }
                    printOriginalExpression();
                }
            }

            void printOriginalExpression() const {
                if( result.hasExpression() ) {
                    stream << " " << result.getExpression();
                }
            }

            void printReconstructedExpression() const {
                if( result.hasExpandedExpression() ) {
                    {
                        Colour colour( dimColour() );
                        stream << " for: ";
                    }
                    stream << result.getExpandedExpression();
                }
            }

            void printMessage() {
                if ( itMessage != messages.end() ) {
                    stream << " '" << itMessage->message << "'";
                    ++itMessage;
                }
            }

            void printRemainingMessages( Colour::Code colour = dimColour() ) {
                if ( itMessage == messages.end() )
                    return;

                // using messages.end() directly yields compilation error:
                std::vector<MessageInfo>::const_iterator itEnd = messages.end();
                const std::size_t N = static_cast<std::size_t>( std::distance( itMessage, itEnd ) );

                {
                    Colour colourGuard( colour );
                    stream << " with " << pluralise( N, "message" ) << ":";
                }

                for(; itMessage != itEnd; ) {
                    // If this assertion is a warning ignore any INFO messages
                    if( printInfoMessages || itMessage->type != ResultWas::Info ) {
                        stream << " '" << itMessage->message << "'";
                        if ( ++itMessage != itEnd ) {
                            Colour colourGuard( dimColour() );
                            stream << " and";
                        }
                    }
                }
            }

        private:
            std::ostream& stream;
            AssertionStats const& stats;
            AssertionResult const& result;
            std::vector<MessageInfo> messages;
            std::vector<MessageInfo>::const_iterator itMessage;
            bool printInfoMessages;
        };

        // Colour, message variants:
        // - white: No tests ran.
        // -   red: Failed [both/all] N test cases, failed [both/all] M assertions.
        // - white: Passed [both/all] N test cases (no assertions).
        // -   red: Failed N tests cases, failed M assertions.
        // - green: Passed [both/all] N tests cases with M assertions.

        std::string bothOrAll( std::size_t count ) const {
            return count == 1 ? "" : count == 2 ? "both " : "all " ;
        }

        void printTotals( const Totals& totals ) const {
            if( totals.testCases.total() == 0 ) {
                stream << "No tests ran.";
            }
            else if( totals.testCases.failed == totals.testCases.total() ) {
                Colour colour( Colour::ResultError );
                const std::string qualify_assertions_failed =
                    totals.assertions.failed == totals.assertions.total() ?
                        bothOrAll( totals.assertions.failed ) : "";
                stream <<
                    "Failed " << bothOrAll( totals.testCases.failed )
                              << pluralise( totals.testCases.failed, "test case"  ) << ", "
                    "failed " << qualify_assertions_failed <<
                                 pluralise( totals.assertions.failed, "assertion" ) << ".";
            }
            else if( totals.assertions.total() == 0 ) {
                stream <<
                    "Passed " << bothOrAll( totals.testCases.total() )
                              << pluralise( totals.testCases.total(), "test case" )
                              << " (no assertions).";
            }
            else if( totals.assertions.failed ) {
                Colour colour( Colour::ResultError );
                stream <<
                    "Failed " << pluralise( totals.testCases.failed, "test case"  ) << ", "
                    "failed " << pluralise( totals.assertions.failed, "assertion" ) << ".";
            }
            else {
                Colour colour( Colour::ResultSuccess );
                stream <<
                    "Passed " << bothOrAll( totals.testCases.passed )
                              << pluralise( totals.testCases.passed, "test case"  ) <<
                    " with "  << pluralise( totals.assertions.passed, "assertion" ) << ".";
            }
        }
    };

    INTERNAL_CATCH_REGISTER_REPORTER( "compact", CompactReporter )

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_COMPACT_HPP_INCLUDED
