
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
/** \file
 * This is a special TU that combines what would otherwise be a very
 * small reporter-related TUs into one bigger TU.
 *
 * The reason for this is compilation performance improvements by
 * avoiding reparsing headers for many small TUs, instead having this
 * one TU include bit more, but having it all parsed only once.
 *
 * To avoid heavy-tail problem with compilation times, each "subpart"
 * of Catch2 has its own combined TU like this.
 */

#include <catch2/reporters/catch_reporter_helpers.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>
#include <catch2/internal/catch_console_width.hpp>
#include <catch2/internal/catch_errno_guard.hpp>
#include <catch2/internal/catch_textflow.hpp>
#include <catch2/internal/catch_stream.hpp>
#include <catch2/internal/catch_string_manip.hpp>
#include <catch2/internal/catch_console_colour.hpp>
#include <catch2/catch_tostring.hpp>
#include <catch2/catch_test_case_info.hpp>

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <ostream>
#include <iomanip>

namespace Catch {

    namespace {
        void listTestNamesOnly(std::ostream& out,
                               std::vector<TestCaseHandle> const& tests) {
            for (auto const& test : tests) {
                auto const& testCaseInfo = test.getTestCaseInfo();

                if (startsWith(testCaseInfo.name, '#')) {
                    out << '"' << testCaseInfo.name << '"';
                } else {
                    out << testCaseInfo.name;
                }

                out << '\n';
            }
            out << std::flush;
        }
    } // end unnamed namespace


    // Because formatting using c++ streams is stateful, drop down to C is
    // required Alternatively we could use stringstream, but its performance
    // is... not good.
    std::string getFormattedDuration( double duration ) {
        // Max exponent + 1 is required to represent the whole part
        // + 1 for decimal point
        // + 3 for the 3 decimal places
        // + 1 for null terminator
        const std::size_t maxDoubleSize = DBL_MAX_10_EXP + 1 + 1 + 3 + 1;
        char buffer[maxDoubleSize];

        // Save previous errno, to prevent sprintf from overwriting it
        ErrnoGuard guard;
#ifdef _MSC_VER
        sprintf_s( buffer, "%.3f", duration );
#else
        std::snprintf( buffer, maxDoubleSize, "%.3f", duration );
#endif
        return std::string( buffer );
    }

    bool shouldShowDuration( IConfig const& config, double duration ) {
        if ( config.showDurations() == ShowDurations::Always ) {
            return true;
        }
        if ( config.showDurations() == ShowDurations::Never ) {
            return false;
        }
        const double min = config.minDuration();
        return min >= 0 && duration >= min;
    }

    std::string serializeFilters( std::vector<std::string> const& filters ) {
        // We add a ' ' separator between each filter
        size_t serialized_size = filters.size() - 1;
        for (auto const& filter : filters) {
            serialized_size += filter.size();
        }

        std::string serialized;
        serialized.reserve(serialized_size);
        bool first = true;

        for (auto const& filter : filters) {
            if (!first) {
                serialized.push_back(' ');
            }
            first = false;
            serialized.append(filter);
        }

        return serialized;
    }

    std::ostream& operator<<( std::ostream& out, lineOfChars value ) {
        for ( size_t idx = 0; idx < CATCH_CONFIG_CONSOLE_WIDTH - 1; ++idx ) {
            out.put( value.c );
        }
        return out;
    }

    void
    defaultListReporters( std::ostream& out,
                          std::vector<ReporterDescription> const& descriptions,
                          Verbosity verbosity ) {
        out << "Available reporters:\n";
        const auto maxNameLen =
            std::max_element( descriptions.begin(),
                              descriptions.end(),
                              []( ReporterDescription const& lhs,
                                  ReporterDescription const& rhs ) {
                                  return lhs.name.size() < rhs.name.size();
                              } )
                ->name.size();

        for ( auto const& desc : descriptions ) {
            if ( verbosity == Verbosity::Quiet ) {
                out << TextFlow::Column( desc.name )
                           .indent( 2 )
                           .width( 5 + maxNameLen )
                    << '\n';
            } else {
                out << TextFlow::Column( desc.name + ":" )
                               .indent( 2 )
                               .width( 5 + maxNameLen ) +
                           TextFlow::Column( desc.description )
                               .initialIndent( 0 )
                               .indent( 2 )
                               .width( CATCH_CONFIG_CONSOLE_WIDTH - maxNameLen - 8 )
                    << '\n';
            }
        }
        out << '\n' << std::flush;
    }

    void defaultListTags( std::ostream& out,
                          std::vector<TagInfo> const& tags,
                          bool isFiltered ) {
        if ( isFiltered ) {
            out << "Tags for matching test cases:\n";
        } else {
            out << "All available tags:\n";
        }

        for ( auto const& tagCount : tags ) {
            ReusableStringStream rss;
            rss << "  " << std::setw( 2 ) << tagCount.count << "  ";
            auto str = rss.str();
            auto wrapper = TextFlow::Column( tagCount.all() )
                               .initialIndent( 0 )
                               .indent( str.size() )
                               .width( CATCH_CONFIG_CONSOLE_WIDTH - 10 );
            out << str << wrapper << '\n';
        }
        out << pluralise( tags.size(), "tag" ) << '\n' << std::endl;
    }

    void defaultListTests(std::ostream& out, std::vector<TestCaseHandle> const& tests, bool isFiltered, Verbosity verbosity) {
        // We special case this to provide the equivalent of old
        // `--list-test-names-only`, which could then be used by the
        // `--input-file` option.
        if (verbosity == Verbosity::Quiet) {
            listTestNamesOnly(out, tests);
            return;
        }

        if (isFiltered) {
            out << "Matching test cases:\n";
        } else {
            out << "All available test cases:\n";
        }

        for (auto const& test : tests) {
            auto const& testCaseInfo = test.getTestCaseInfo();
            Colour::Code colour = testCaseInfo.isHidden()
                ? Colour::SecondaryText
                : Colour::None;
            Colour colourGuard(colour);

            out << TextFlow::Column(testCaseInfo.name).initialIndent(2).indent(4) << '\n';
            if (verbosity >= Verbosity::High) {
                out << TextFlow::Column(Catch::Detail::stringify(testCaseInfo.lineInfo)).indent(4) << std::endl;
            }
            if (!testCaseInfo.tags.empty() &&
                verbosity > Verbosity::Quiet) {
                out << TextFlow::Column(testCaseInfo.tagsAsString()).indent(6) << '\n';
            }
        }

        if (isFiltered) {
            out << pluralise(tests.size(), "matching test case") << '\n' << std::endl;
        } else {
            out << pluralise(tests.size(), "test case") << '\n' << std::endl;
        }
    }

} // namespace Catch


#include <catch2/reporters/catch_reporter_event_listener.hpp>

namespace Catch {
    void EventListenerBase::assertionStarting( AssertionInfo const& ) {}

    bool EventListenerBase::assertionEnded( AssertionStats const& ) {
        return false;
    }
    void EventListenerBase::listReporters(
        std::vector<ReporterDescription> const& ) {}
    void EventListenerBase::listTests( std::vector<TestCaseHandle> const& ) {}
    void EventListenerBase::listTags( std::vector<TagInfo> const& ) {}
    void EventListenerBase::noMatchingTestCases( std::string const& ) {}
    void EventListenerBase::testRunStarting( TestRunInfo const& ) {}
    void EventListenerBase::testGroupStarting( GroupInfo const& ) {}
    void EventListenerBase::testCaseStarting( TestCaseInfo const& ) {}
    void EventListenerBase::sectionStarting( SectionInfo const& ) {}
    void EventListenerBase::sectionEnded( SectionStats const& ) {}
    void EventListenerBase::testCaseEnded( TestCaseStats const& ) {}
    void EventListenerBase::testGroupEnded( TestGroupStats const& ) {}
    void EventListenerBase::testRunEnded( TestRunStats const& ) {}
    void EventListenerBase::skipTest( TestCaseInfo const& ) {}
} // namespace Catch
