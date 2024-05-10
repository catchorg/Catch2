
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
        std::ostream& tagWriter( std::ostream& out,
                                 std::vector<Tag> const& tags ) {
            out << ';';
            if ( tags.empty() ) { return out; }

            out << "[=[" << "[==[" << tags.front().original << "]==]";
            for ( auto it = std::next( tags.begin() ); it != tags.end();
                  ++it ) {
                out << ';' << "[==[" << it->original << "]==]";
            }
            out << "]=]";
            return out;
        }

        std::ostream& testWriter( std::ostream& out,
                                  TestCaseHandle const& test ) {
            auto const& info = test.getTestCaseInfo();
            out << "[[" << "[=[" << info.name << "]=]" << ';' << "[=["
                << info.className << "]=]" << ';' << "[=[" << info.lineInfo.file
                << "]=]" << ';' << "[=[" << info.lineInfo.line << "]=]";
            tagWriter( out, info.tags );
            out << "]]";

            return out;
        }

    } // namespace

    std::string CMakeReporter::getDescription() {
        return "Outputs listings as a CMake list";
    }

    void CMakeReporter::listReporters(
        std::vector<ReporterDescription> const& descriptions ) {

        if ( descriptions.empty() ) { return; }

        m_stream << "[[" << "[=[" << descriptions.front().name << "]=]" << ';'
                 << "[=[" << descriptions.front().description << "]=]" << "]]";
        for ( auto it = std::next( descriptions.begin() );
              it != descriptions.end();
              ++it ) {
            m_stream << ';' << "[[" << "[=[" << it->name << "]=]" << ';'
                     << "[=[" << it->description << "]=]" << "]]";
        }

        m_stream << '\n';
    }

    void CMakeReporter::listListeners(
        std::vector<ListenerDescription> const& descriptions ) {

        if ( descriptions.empty() ) { return; }

        m_stream << "[[" << "[=[" << descriptions.front().name << "]=]" << ';'
                 << "[=[" << descriptions.front().description << "]=]" << "]]";
        for ( auto it = std::next( descriptions.begin() );
              it != descriptions.end();
              ++it ) {
            m_stream << ';' << "[[" << "[=[" << it->name << "]=]" << ';'
                     << "[=[" << it->description << "]=]" << "]]";
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

        m_stream << "[[" << "[=[" << tags.front().count << "]=]" << ';' << "[=["
                 << tags.front().all() << "]=]" << "]]";

        for ( auto it = std::next( tags.begin() ); it != tags.end(); ++it ) {
            m_stream << ';' << "[[" << "[=[" << it->count << "]=]" << ';'
                     << "[=[" << it->all() << "]=]" << "]]";
        }

        m_stream << '\n';
    }

} // end namespace Catch
