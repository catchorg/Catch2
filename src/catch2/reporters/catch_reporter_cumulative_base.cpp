
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/reporters/catch_reporter_cumulative_base.hpp>
#include <catch2/reporters/catch_reporter_helpers.hpp>

#include <algorithm>
#include <cassert>

namespace Catch {
    namespace {
        struct BySectionInfo {
            BySectionInfo( SectionInfo const& other ): m_other( other ) {}
            BySectionInfo( BySectionInfo const& other ):
                m_other( other.m_other ) {}
            bool operator()(
                Detail::unique_ptr<CumulativeReporterBase::SectionNode> const&
                    node ) const {
                return (
                    ( node->stats.sectionInfo.name == m_other.name ) &&
                    ( node->stats.sectionInfo.lineInfo == m_other.lineInfo ) );
            }
            void operator=( BySectionInfo const& ) = delete;

        private:
            SectionInfo const& m_other;
        };

    } // namespace


    CumulativeReporterBase::~CumulativeReporterBase() = default;

    void
    CumulativeReporterBase::sectionStarting( SectionInfo const& sectionInfo ) {
        SectionStats incompleteStats( sectionInfo, Counts(), 0, false );
        SectionNode* node;
        if ( m_sectionStack.empty() ) {
            if ( !m_rootSection ) {
                m_rootSection =
                    Detail::make_unique<SectionNode>( incompleteStats );
            }
            node = m_rootSection.get();
        } else {
            SectionNode& parentNode = *m_sectionStack.back();
            auto it = std::find_if( parentNode.childSections.begin(),
                                    parentNode.childSections.end(),
                                    BySectionInfo( sectionInfo ) );
            if ( it == parentNode.childSections.end() ) {
                auto newNode =
                    Detail::make_unique<SectionNode>( incompleteStats );
                node = newNode.get();
                parentNode.childSections.push_back( std::move( newNode ) );
            } else {
                node = it->get();
            }
        }

        m_deepestSection = node;
        m_sectionStack.push_back( node );
    }

    bool CumulativeReporterBase::assertionEnded(
        AssertionStats const& assertionStats ) {
        assert( !m_sectionStack.empty() );
        // AssertionResult holds a pointer to a temporary DecomposedExpression,
        // which getExpandedExpression() calls to build the expression string.
        // Our section stack copy of the assertionResult will likely outlive the
        // temporary, so it must be expanded or discarded now to avoid calling
        // a destroyed object later.
        static_cast<void>(
            assertionStats.assertionResult.getExpandedExpression() );
        SectionNode& sectionNode = *m_sectionStack.back();
        sectionNode.assertions.push_back( assertionStats );
        return true;
    }

    void CumulativeReporterBase::sectionEnded( SectionStats const& sectionStats ) {
        assert( !m_sectionStack.empty() );
        SectionNode& node = *m_sectionStack.back();
        node.stats = sectionStats;
        m_sectionStack.pop_back();
    }

    void CumulativeReporterBase::testCaseEnded(
        TestCaseStats const& testCaseStats ) {
        auto node = Detail::make_unique<TestCaseNode>( testCaseStats );
        assert( m_sectionStack.size() == 0 );
        node->children.push_back( std::move(m_rootSection) );
        m_testCases.push_back( std::move(node) );

        assert( m_deepestSection );
        m_deepestSection->stdOut = testCaseStats.stdOut;
        m_deepestSection->stdErr = testCaseStats.stdErr;
    }

    void CumulativeReporterBase::testGroupEnded(
        TestGroupStats const& testGroupStats ) {
        auto node = Detail::make_unique<TestGroupNode>( testGroupStats );
        node->children.swap( m_testCases );
        m_testGroups.push_back( std::move(node) );
    }

    void CumulativeReporterBase::testRunEnded( TestRunStats const& testRunStats ) {
        m_testRuns.emplace_back( testRunStats );
        m_testRuns.back().children.swap( m_testGroups );
        testRunEndedCumulative();
    }

    void CumulativeReporterBase::listReporters(std::vector<ReporterDescription> const& descriptions) {
        defaultListReporters(stream, descriptions, m_config->verbosity());
    }

    void CumulativeReporterBase::listTests(std::vector<TestCaseHandle> const& tests) {
        defaultListTests(stream,
                         tests,
                         m_config->hasTestFilters(),
                         m_config->verbosity());
    }

    void CumulativeReporterBase::listTags(std::vector<TagInfo> const& tags) {
        defaultListTags( stream, tags, m_config->hasTestFilters() );
    }

} // end namespace Catch
