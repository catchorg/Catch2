/*
 *  Created by Phil on 5/12/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_reporter_gtest.h"

#include "../internal/catch_console_colour.h"
#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_stringref.h"
#include "../internal/catch_text.h"
#include "../internal/catch_version.h"

#include <cfloat>
#include <cmath>
#include <cstdio>

#if defined( _MSC_VER )
#    pragma warning( push )
#    pragma warning( \
        disable : 4061 ) // Not all labels are EXPLICITLY handled in switch
// Note that 4062 (not all labels are handled and default is missing) is enabled
#endif

#if defined( __clang__ )
#    pragma clang diagnostic push
// For simplicity, benchmarking-only helpers are always enabled
#    pragma clang diagnostic ignored "-Wunused-function"
#endif

namespace Catch {

    namespace {

        struct Coloured {
            std::string text;
            Colour::Code code;
        };

        std::ostream& operator<<( std::ostream& os, Coloured coloured ) {
            Colour colourGuard( coloured.code );
            return os << coloured.text;
        }

        std::string formatDuration( double durationInSeconds ) {
            return std::to_string( static_cast<int64_t>(
                       std::round( durationInSeconds * 1000 ) ) ) +
                   " ms";
        }

        // Formatter impl for GTestReporter
        class ConsoleAssertionPrinter {
        public:
            ConsoleAssertionPrinter&
            operator=( ConsoleAssertionPrinter const& ) = delete;
            ConsoleAssertionPrinter( ConsoleAssertionPrinter const& ) = delete;
            ConsoleAssertionPrinter( std::ostream& _stream,
                                     AssertionStats const& _stats,
                                     bool _printInfoMessages ):
                stream( _stream ),
                stats( _stats ),
                result( _stats.assertionResult ),
                colour( Colour::None ),
                message( result.getMessage() ),
                messages( _stats.infoMessages ),
                printInfoMessages( _printInfoMessages ) {
                switch ( result.getResultType() ) {
                case ResultWas::Ok:
                    colour = Colour::Success;
                    passOrFail = "PASSED";
                    // if( result.hasMessage() )
                    if ( _stats.infoMessages.size() == 1 )
                        messageLabel = "with message";
                    if ( _stats.infoMessages.size() > 1 )
                        messageLabel = "with messages";
                    break;
                case ResultWas::ExpressionFailed:
                    if ( result.isOk() ) {
                        colour = Colour::Success;
                        passOrFail = "FAILED - but was ok";
                    } else {
                        colour = Colour::Error;
                        passOrFail = "FAILED";
                    }
                    if ( _stats.infoMessages.size() == 1 )
                        messageLabel = "with message";
                    if ( _stats.infoMessages.size() > 1 )
                        messageLabel = "with messages";
                    break;
                case ResultWas::ThrewException:
                    colour = Colour::Error;
                    passOrFail = "FAILED";
                    messageLabel = "due to unexpected exception with ";
                    if ( _stats.infoMessages.size() == 1 )
                        messageLabel += "message";
                    if ( _stats.infoMessages.size() > 1 )
                        messageLabel += "messages";
                    break;
                case ResultWas::FatalErrorCondition:
                    colour = Colour::Error;
                    passOrFail = "FAILED";
                    messageLabel = "due to a fatal error condition";
                    break;
                case ResultWas::DidntThrowException:
                    colour = Colour::Error;
                    passOrFail = "FAILED";
                    messageLabel = "because no exception was thrown where one "
                                   "was expected";
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
                    if ( _stats.infoMessages.size() == 1 )
                        messageLabel = "explicitly with message";
                    if ( _stats.infoMessages.size() > 1 )
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
                if ( stats.totals.assertions.total() > 0 ) {
                    printResultType();
                    printOriginalExpression();
                    printReconstructedExpression();
                } else {
                    stream << '\n';
                }
                printMessage();
            }

        private:
            void printResultType() const {
                if ( !passOrFail.empty() ) {
                    Colour colourGuard( colour );
                    stream << passOrFail << ":\n";
                }
            }
            void printOriginalExpression() const {
                if ( result.hasExpression() ) {
                    Colour colourGuard( Colour::OriginalExpression );
                    stream << "  ";
                    stream << result.getExpressionInMacro();
                    stream << '\n';
                }
            }
            void printReconstructedExpression() const {
                if ( result.hasExpandedExpression() ) {
                    stream << "with expansion:\n";
                    Colour colourGuard( Colour::ReconstructedExpression );
                    stream
                        << Column( result.getExpandedExpression() ).indent( 2 )
                        << '\n';
                }
            }
            void printMessage() const {
                if ( !messageLabel.empty() )
                    stream << messageLabel << ':' << '\n';
                for ( auto const& msg : messages ) {
                    // If this assertion is a warning ignore any INFO messages
                    if ( printInfoMessages || msg.type != ResultWas::Info )
                        stream << Column( msg.message ).indent( 2 ) << '\n';
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
    } // namespace

    GTestReporter::GTestReporter( ReporterConfig const& config ):
        StreamingReporterBase( config ) {}

    GTestReporter::~GTestReporter() = default;

    std::string GTestReporter::getDescription() {
        return "Reports each test and section as it runs, similar to Google "
               "Test. Entering the root section corresponding to the test body "
               "is not reported for each subsection by default, set verbosity "
               "to high to change this.";
    }

    std::set<Verbosity> GTestReporter::getSupportedVerbosities() {
        return { Verbosity::Normal, Verbosity::High };
    }

    void GTestReporter::noMatchingTestCases( std::string const& spec ) {
        stream << "No test cases matched '" << spec << '\'' << std::endl;
    }

    void GTestReporter::reportInvalidArguments( std::string const& arg ) {
        stream << "Invalid Filter: " << arg << std::endl;
    }

    void GTestReporter::testRunStarting( TestRunInfo const& _testInfo ) {
        StreamingReporterBase::testRunStarting( _testInfo );
        printTestFilters();
        m_runStats.timer.start();
        stream << Coloured{ "[==========] ", Colour::Green } << "Running "
               << _testInfo.name << "\n";
    }

    void GTestReporter::testCaseStarting( TestCaseInfo const& _testInfo ) {
        StreamingReporterBase::testCaseStarting( _testInfo );
        m_testCaseStats = {};
        m_testCaseStats.timer.start();
        m_prevNestedSectionAssertionFailures = 0;
        stream << Coloured{ "[----------] ", Colour::Green } << _testInfo.name
               << "\n";
        if ( shouldPrintRootSectionOnlyOnce() ) {
            stream << Coloured{ "[ RUN      ] ", Colour::Green }
                   << _testInfo.name << "\n";
        }
    }

    void GTestReporter::sectionStarting( SectionInfo const& _sectionInfo ) {
        StreamingReporterBase::sectionStarting( _sectionInfo );
        if ( m_sectionStack.size() > 1 || !shouldPrintRootSectionOnlyOnce() ) {
            stream << Coloured{ "[ RUN      ] ", Colour::Green }
                   << formatFullSectionName() << "\n";
        }
    }

    void GTestReporter::assertionStarting( AssertionInfo const& ) {}

    bool
    GTestReporter::assertionEnded( AssertionStats const& _assertionStats ) {
        AssertionResult const& result = _assertionStats.assertionResult;

        bool includeResults =
            m_config->includeSuccessfulResults() || !result.isOk();

        // Drop out if result was successful but we're not printing them.
        if ( !includeResults && result.getResultType() != ResultWas::Warning )
            return false;

        ConsoleAssertionPrinter printer(
            stream, _assertionStats, includeResults );
        printer.print();
        stream << std::endl;
        return true;
    }

    void GTestReporter::sectionEnded( SectionStats const& _sectionStats ) {
        if ( _sectionStats.missingAssertions &&
             m_config->warnAboutMissingAssertions() ) {
            Colour colour( Colour::Warning );
            if ( m_sectionStack.size() > 1 )
                stream << "No assertions in section\n";
            else
                stream << "No assertions in test case\n";
        }
        const auto sectionName = formatFullSectionName();
        if ( m_sectionStack.size() > 1 || !shouldPrintRootSectionOnlyOnce() ) {
            stream << ( _sectionStats.assertions.allOk()
                            ? Coloured{ "[       OK ] ", Colour::Green }
                            : Coloured{ "[  FAILED  ] ", Colour::Red } )
                   << sectionName << " ("
                   << formatDuration( _sectionStats.durationInSeconds )
                   << ")\n";
        }
        if ( m_sectionStack.size() == 1 ) {
            if ( _sectionStats.assertions.allPassed() ) {
                m_testCaseStats.tests.passed++;
            } else if ( _sectionStats.assertions.allOk() ) {
                m_testCaseStats.tests.failedButOk++;
            } else {
                m_testCaseStats.tests.failed++;
            }
        }
        if ( _sectionStats.assertions.failed >
             m_prevNestedSectionAssertionFailures ) {
            if ( !m_failedSectionsSet.count( sectionName ) ) {
                m_failedSections.emplace_back( sectionName );
                m_failedSectionsSet.insert( sectionName );
            }
        }
        m_prevNestedSectionAssertionFailures = _sectionStats.assertions.failed;
        StreamingReporterBase::sectionEnded( _sectionStats );
    }

    void GTestReporter::testCaseEnded( TestCaseStats const& _testCaseStats ) {
        StreamingReporterBase::testCaseEnded( _testCaseStats );
        m_runStats.tests += m_testCaseStats.tests;
        if ( shouldPrintRootSectionOnlyOnce() ) {
            stream << ( _testCaseStats.totals.testCases.allOk()
                            ? Coloured{ "[       OK ] ", Colour::Green }
                            : Coloured{ "[  FAILED  ] ", Colour::Red } )
                   << _testCaseStats.testInfo.name << "\n";
        }
        stream << Coloured{ "[----------] ", Colour::Green }
               << pluralise( m_testCaseStats.tests.total(), "test" ) << " from "
               << _testCaseStats.testInfo.name << " ("
               << formatDuration( m_testCaseStats.timer.getElapsedSeconds() )
               << " total)\n";
        if ( m_testCaseStats.tests.failedButOk > 0 ) {
            stream << Coloured{ "[ EXPECTED ] ", Colour::Yellow }
                   << pluralise( m_testCaseStats.tests.failedButOk, "test" )
                   << " failed as expected\n";
        }
        if ( m_testCaseStats.tests.failed > 0 ) {
            stream << Coloured{ "[  FAILED  ] ", Colour::Red }
                   << pluralise( m_testCaseStats.tests.failed, "test" )
                   << " failed\n";
        }
        stream << "\n";
    }

    void GTestReporter::testRunEnded( TestRunStats const& _testRunStats ) {
        StreamingReporterBase::testRunEnded( _testRunStats );
        stream << Coloured{ "[==========] ", Colour::Green }
               << pluralise( m_runStats.tests.total(), "test" ) << " from "
               << pluralise( _testRunStats.totals.testCases.total(),
                             "test case" )
               << " run ("
               << formatDuration( m_runStats.timer.getElapsedSeconds() )
               << " total)\n";
        stream << Coloured{ "[  PASSED  ] ", Colour::Green }
               << pluralise( m_runStats.tests.passed, "test" ) << " passed\n";
        if ( m_runStats.tests.failedButOk > 0 ) {
            stream << Coloured{ "[ EXPECTED ] ", Colour::Yellow }
                   << pluralise( m_runStats.tests.failedButOk, "test" )
                   << " failed as expected\n";
        }
        if ( m_runStats.tests.failed > 0 ) {
            stream << Coloured{ "[  FAILED  ] ", Colour::Red }
                   << pluralise( m_runStats.tests.failed, "test" )
                   << " failed in "
                   << pluralise( m_failedSections.size(), "section" ) << ":\n";
            for ( const auto& section : m_failedSections ) {
                stream << Coloured{ "[  FAILED  ] ", Colour::Red } << section
                       << "\n";
            }
            stream << "\n "
                   << toUpper(
                          pluralise( m_runStats.tests.failed, "failed test" )
                              .str() )
                   << "\n";
        }
    }

    bool GTestReporter::shouldPrintRootSectionOnlyOnce() const {
        return m_config->verbosity() <= Verbosity::Normal;
    }

    std::string GTestReporter::formatFullSectionName() {
        std::stringstream ss;
        bool need_separator = false;
        for ( const auto& section : m_sectionStack ) {
            if ( need_separator ) {
                ss << " / ";
            } else {
                need_separator = true;
            }
            ss << trim( section.name );
        }
        return ss.str();
    }

    void GTestReporter::printTestFilters() {
        if ( m_config->testSpec().hasFilters() ) {
            Colour guard( Colour::Yellow );
            stream << "Filters: "
                   << serializeFilters( m_config->getTestsOrTags() ) << '\n';
        }
    }

    CATCH_REGISTER_REPORTER( "gtest", GTestReporter )

} // end namespace Catch

#if defined( _MSC_VER )
#    pragma warning( pop )
#endif

#if defined( __clang__ )
#    pragma clang diagnostic pop
#endif
