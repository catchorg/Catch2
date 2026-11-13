//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/reporters/catch_reporter_cumulative_base.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include <ostream>
#include <set>
#include <string>
#include <vector>

class SectionTreeReporter final : public Catch::CumulativeReporterBase {
    std::vector<std::string> m_names;
    std::set<std::string> m_expectedPaths;
    bool m_endNamesMatch = true;

    static std::string path( std::vector<std::string> const& names ) {
        std::string result;
        for ( auto const& name : names ) {
            result += '/' + name;
        }
        return result;
    }

    static void collectPaths( SectionNode const& node,
                              std::string const& prefix,
                              std::set<std::string>& paths ) {
        auto current = prefix + '/' + node.stats.sectionInfo.name;
        paths.insert( current );
        for ( auto const& child : node.childSections ) {
            collectPaths( *child, current, paths );
        }
    }

public:
    explicit SectionTreeReporter( Catch::ReporterConfig&& config ):
        CumulativeReporterBase( CATCH_MOVE( config ) ) {}

    static std::string getDescription() {
        return "Checks cumulative section hierarchy against started sections";
    }

    void sectionStarting( Catch::SectionInfo const& info ) override {
        CumulativeReporterBase::sectionStarting( info );
        m_names.push_back( info.name );
        m_expectedPaths.insert( path( m_names ) );
    }

    void sectionEnded( Catch::SectionStats const& stats ) override {
        CumulativeReporterBase::sectionEnded( stats );
        m_endNamesMatch = m_endNamesMatch && !m_names.empty() &&
                          m_names.back() == stats.sectionInfo.name;
        m_names.pop_back();
    }

    void testRunEndedCumulative() override {
        std::set<std::string> actualPaths;
        for ( auto const& test : m_testRun->children ) {
            for ( auto const& section : test->children ) {
                collectPaths( *section, "", actualPaths );
            }
        }
        bool valid = m_endNamesMatch && m_names.empty() &&
                     m_expectedPaths.size() >= 4 &&
                     actualPaths == m_expectedPaths;
        m_stream << ( valid ? "CUMULATIVE_TREE_OK" : "CUMULATIVE_TREE_BAD" )
                 << '\n';
    }
};

CATCH_REGISTER_REPORTER( "section-tree", SectionTreeReporter )

static void nestedSections( int mode ) {
    SECTION( "outer" ) {
        SECTION( "passing child" ) { SUCCEED(); }
        SECTION( "early child" ) {
            SECTION( "innermost" ) {
                if ( mode == 0 ) {
                    SKIP( "nested skip" );
                } else if ( mode == 1 ) {
                    FAIL( "nested fatal assertion" );
                } else if ( mode == 2 ) {
                    CATCH_RUNTIME_ERROR( "nested exception" );
                } else {
                    SUCCEED();
                }
            }
        }
        SECTION( "later sibling" ) { SUCCEED(); }
    }
}

TEST_CASE( "nested skip" ) { nestedSections( 0 ); }
TEST_CASE( "nested fatal assertion" ) { nestedSections( 1 ); }
TEST_CASE( "nested exception" ) { nestedSections( 2 ); }
TEST_CASE( "normal nested sections" ) { nestedSections( 3 ); }
