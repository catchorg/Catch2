
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/catch_test_case_info.hpp>
#include <catch2/internal/catch_reusable_string_stream.hpp>
#include <catch2/internal/catch_stringref.hpp>
#include <catch2/reporters/catch_reporter_cmake.hpp>
#include <catch2/reporters/catch_reporter_helpers.hpp>

#include <ostream>

namespace Catch {

    namespace {
        template <typename T>
        std::string escapeString( T const& str ) {
            ReusableStringStream rss;
            for ( auto const& c : str ) {
                switch ( c ) {
                case ';':
                    rss << R"(\\;)";
                    break;
                default:
                    rss << c;
                    break;
                }
            }

            return rss.str();
        }

        std::string escapeCString( const char* const& str ) {
            return escapeString( std::string( str ) );
        }

        std::ostream& tagWriter( std::ostream& out,
                                 std::vector<Tag> const& tags ) {
            if ( tags.empty() ) {
                out << "\\;";
                return out;
            }
            out << "\\;" << escapeString( tags.front().original );
            for ( auto it = std::next( tags.begin() ); it != tags.end();
                  ++it ) {
                out << ',' << escapeString( it->original );
            }
            return out;
        }

        std::ostream& testWriter( std::ostream& out,
                                  TestCaseHandle const& test ) {
            auto const& info = test.getTestCaseInfo();
            out << escapeString( info.name ) << "\\;"
                << escapeString( info.className ) << "\\;"
                << escapeCString( info.lineInfo.file ) << "\\;"
                << info.lineInfo.line;
            tagWriter( out, info.tags );

            return out;
        }

    } // namespace

    std::string CMakeReporter::getDescription() {
        return "Outputs listings as a CMake list";
    }

    void CMakeReporter::listReporters(
        std::vector<ReporterDescription> const& descriptions ) {

        if ( descriptions.empty() ) { return; }

        m_stream << descriptions.front().name << "\\;"
                 << descriptions.front().description;
        for ( auto it = std::next( descriptions.begin() );
              it != descriptions.end();
              ++it ) {
            m_stream << ";" << escapeString( it->name ) << "\\;"
                     << escapeString( it->description );
        }

        m_stream << '\n';
    }

    void CMakeReporter::listListeners(
        std::vector<ListenerDescription> const& descriptions ) {

        if ( descriptions.empty() ) { return; }

        m_stream << descriptions.front().name << "\\;"
                 << descriptions.front().description;
        for ( auto it = std::next( descriptions.begin() );
              it != descriptions.end();
              ++it ) {
            m_stream << ";" << escapeString( it->name ) << "\\;"
                     << escapeString( it->description );
        }

        m_stream << '\n';
    }

    void CMakeReporter::listTests( std::vector<TestCaseHandle> const& tests ) {
        if ( tests.empty() ) { return; }

        testWriter( m_stream, tests.front() );

        for ( auto it = std::next( tests.begin() ); it != tests.end(); ++it ) {
            m_stream << ';';
            testWriter( m_stream, *it );
        }

        m_stream << '\n';
    }

    void CMakeReporter::listTags( std::vector<TagInfo> const& tags ) {
        if ( tags.empty() ) { return; }

        m_stream << tags.front().count << "\\;"
                 << escapeString( tags.front().all() );

        for ( auto it = std::next( tags.begin() ); it != tags.end(); ++it ) {
            m_stream << ";" << it->count << "\\;" << escapeString( it->all() );
        }

        m_stream << '\n';
    }

} // end namespace Catch
