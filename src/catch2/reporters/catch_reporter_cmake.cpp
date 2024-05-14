
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
        std::string createOpenBracket( int const& indent ) {
            ReusableStringStream rss;
            rss << '[';
            if ( indent > 0 ) { rss << std::string( indent, '=' ); }
            rss << '[';
            return rss.str();
        }
        std::string createCloseBracket( int const& indent ) {
            ReusableStringStream rss;
            rss << ']';
            if ( indent > 0 ) { rss << std::string( indent, '=' ); }
            rss << ']';
            return rss.str();
        }
        std::ostream& tagWriter( std::ostream& out,
                                 std::vector<Tag> const& tags,
                                 int const& indent ) {
            out << ';';
            if ( tags.empty() ) { return out; }

            out << createOpenBracket( indent );

            out << createOpenBracket( indent + 1 ) << tags.front().original
                << createCloseBracket( indent + 1 );
            for ( auto it = std::next( tags.begin() ); it != tags.end();
                  ++it ) {
                out << ';' << createOpenBracket( indent + 1 ) << it->original
                    << createCloseBracket( indent + 1 );
            }

            out << createCloseBracket( indent );

            return out;
        }

        std::ostream& testWriter( std::ostream& out,
                                  TestCaseHandle const& test,
                                  int const& indent ) {
            auto const& info = test.getTestCaseInfo();
            out << createOpenBracket( indent )
                << createOpenBracket( indent + 1 ) << info.name
                << createCloseBracket( indent + 1 ) << ';' << info.className
                << ';' << createOpenBracket( indent + 1 ) << info.lineInfo.file
                << createCloseBracket( indent + 1 ) << ';'
                << info.lineInfo.line;
            tagWriter( out, info.tags, indent );
            out << createCloseBracket( indent );

            return out;
        }

    } // namespace

    std::string CMakeReporter::getDescription() {
        return "Outputs listings as a CMake list";
    }

    void CMakeReporter::listReporters(
        std::vector<ReporterDescription> const& descriptions ) {

        if ( descriptions.empty() ) { return; }

        int indent = 0;

        m_stream << createOpenBracket( indent );

        m_stream << createOpenBracket( indent + 1 )
                 << createOpenBracket( indent + 2 ) << descriptions.front().name
                 << createCloseBracket( indent + 2 ) << ';'
                 << createOpenBracket( indent + 2 )
                 << descriptions.front().description
                 << createCloseBracket( indent + 2 )
                 << createCloseBracket( indent + 1 );
        for ( auto it = std::next( descriptions.begin() );
              it != descriptions.end();
              ++it ) {
            m_stream << ';' << createOpenBracket( indent + 1 )
                     << createOpenBracket( indent + 2 ) << it->name
                     << createCloseBracket( indent + 2 ) << ';'
                     << createOpenBracket( indent + 2 ) << it->description
                     << createCloseBracket( indent + 2 )
                     << createCloseBracket( indent + 1 );
        }

        m_stream << createCloseBracket( indent );
    }

    void CMakeReporter::listListeners(
        std::vector<ListenerDescription> const& descriptions ) {

        if ( descriptions.empty() ) { return; }

        int indent = 0;

        m_stream << createOpenBracket( indent );

        m_stream << createOpenBracket( indent + 1 )
                 << createOpenBracket( indent + 2 ) << descriptions.front().name
                 << createCloseBracket( indent + 2 ) << ';'
                 << createOpenBracket( indent + 2 )
                 << descriptions.front().description
                 << createCloseBracket( indent + 2 )
                 << createCloseBracket( indent + 1 );
        for ( auto it = std::next( descriptions.begin() );
              it != descriptions.end();
              ++it ) {
            m_stream << ';' << createOpenBracket( indent + 1 )
                     << createOpenBracket( indent + 2 ) << it->name
                     << createCloseBracket( indent + 2 ) << ';'
                     << createOpenBracket( indent + 2 ) << it->description
                     << createCloseBracket( indent + 2 )
                     << createCloseBracket( indent + 1 );
        }

        m_stream << createCloseBracket( indent );
    }

    void CMakeReporter::listTests( std::vector<TestCaseHandle> const& tests ) {
        if ( tests.empty() ) { return; }

        int indent = 0;

        m_stream << createOpenBracket( indent );

        testWriter( m_stream, tests.front(), indent + 1 );

        for ( auto it = std::next( tests.begin() ); it != tests.end(); ++it ) {
            m_stream << ';';
            testWriter( m_stream, *it, indent + 1 );
        }

        m_stream << createCloseBracket( indent );
    }

    void CMakeReporter::listTags( std::vector<TagInfo> const& tags ) {
        if ( tags.empty() ) { return; }

        int indent = 0;

        m_stream << createOpenBracket( indent );

        m_stream << createOpenBracket( indent + 1 ) << tags.front().count << ';'
                 << createOpenBracket( indent + 2 ) << tags.front().all()
                 << createCloseBracket( indent + 2 )
                 << createCloseBracket( indent + 1 );

        for ( auto it = std::next( tags.begin() ); it != tags.end(); ++it ) {
            m_stream << ';' << createOpenBracket( indent + 1 ) << it->count
                     << ';' << createOpenBracket( indent + 2 ) << it->all()
                     << createCloseBracket( indent + 2 )
                     << createCloseBracket( indent + 1 );
        }

        m_stream << createCloseBracket( indent );
    }

} // end namespace Catch
