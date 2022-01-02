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
        stream << Coloured{ "[----------] ", Colour::Green } << _testInfo.name
               << "\n";
        if ( shouldPrintRootSectionOnlyOnce() ) {
            stream << Coloured{ "[ RUN      ] ", Colour::Green }
                   << _testInfo.name << "\n";
        }
    }

    void GTestReporter::sectionStarting( SectionInfo const& _sectionInfo ) {
        StreamingReporterBase::sectionStarting( _sectionInfo );
        m_prevNestedSectionAssertionFailures = 0;
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
        if ( _sectionStats.missingAssertions ) {
            Colour colour( Colour::ResultError );
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
                   << " from "
                   << pluralise( _testRunStats.totals.testCases.failed, "test case" )
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
