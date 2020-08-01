/*
 *  Created by Phil on 27/11/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <catch2/internal/catch_console_width.hpp>
#include <catch2/internal/catch_errno_guard.hpp>
#include <catch2/reporters/catch_reporter_bases.hpp>
#include <catch2/internal/catch_stream.hpp>
#include <catch2/catch_config.hpp>

#include <algorithm>
#include <cstring>
#include <cfloat>
#include <cstdio>
#include <ostream>
#include <cassert>

namespace Catch {
    namespace {
        struct BySectionInfo {
            BySectionInfo( SectionInfo const& other ): m_other( other ) {}
            BySectionInfo( BySectionInfo const& other ):
                m_other( other.m_other ) {}
            bool operator()(
                std::shared_ptr<CumulativeReporterBase::SectionNode> const&
                    node ) const {
                return (
                    ( node->stats.sectionInfo.name == m_other.name ) &&
                    ( node->stats.sectionInfo.lineInfo == m_other.lineInfo ) );
            }
            void operator=( BySectionInfo const& ) = delete;

        private:
            SectionInfo const& m_other;
        };

        void prepareExpandedExpression( AssertionResult& result ) {
            result.getExpandedExpression();
        }
    } // namespace

    // Because formatting using c++ streams is stateful, drop down to C is required
    // Alternatively we could use stringstream, but its performance is... not good.
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
        sprintf_s(buffer, "%.3f", duration);
#else
        std::sprintf(buffer, "%.3f", duration);
#endif
        return std::string(buffer);
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

    std::string serializeFilters( std::vector<std::string> const& container ) {
        ReusableStringStream oss;
        bool first = true;
        for (auto&& filter : container)
        {
            if (!first)
                oss << ' ';
            else
                first = false;

            oss << filter;
        }
        return oss.str();
    }

    TestEventListenerBase::TestEventListenerBase(ReporterConfig const & _config)
        :StreamingReporterBase(_config) {}

    void TestEventListenerBase::assertionStarting(AssertionInfo const &) {}

    bool TestEventListenerBase::assertionEnded(AssertionStats const &) {
        return false;
    }


    StreamingReporterBase::~StreamingReporterBase() = default;

    void StreamingReporterBase::testGroupEnded( TestGroupStats const& ) {
        currentGroupInfo.reset();
    }

    void StreamingReporterBase::testRunEnded( TestRunStats const& ) {
        currentTestCaseInfo = nullptr;
        currentGroupInfo.reset();
        currentTestRunInfo.reset();
    }


    CumulativeReporterBase::~CumulativeReporterBase() = default;

    void
    CumulativeReporterBase::sectionStarting( SectionInfo const& sectionInfo ) {
        SectionStats incompleteStats( sectionInfo, Counts(), 0, false );
        std::shared_ptr<SectionNode> node;
        if ( m_sectionStack.empty() ) {
            if ( !m_rootSection )
                m_rootSection =
                    std::make_shared<SectionNode>( incompleteStats );
            node = m_rootSection;
        } else {
            SectionNode& parentNode = *m_sectionStack.back();
            auto it = std::find_if( parentNode.childSections.begin(),
                                    parentNode.childSections.end(),
                                    BySectionInfo( sectionInfo ) );
            if ( it == parentNode.childSections.end() ) {
                node = std::make_shared<SectionNode>( incompleteStats );
                parentNode.childSections.push_back( node );
            } else {
                node = *it;
            }
        }
        m_sectionStack.push_back( node );
        m_deepestSection = std::move( node );
    }

    bool CumulativeReporterBase::assertionEnded(
        AssertionStats const& assertionStats ) {
        assert( !m_sectionStack.empty() );
        // AssertionResult holds a pointer to a temporary DecomposedExpression,
        // which getExpandedExpression() calls to build the expression string.
        // Our section stack copy of the assertionResult will likely outlive the
        // temporary, so it must be expanded or discarded now to avoid calling
        // a destroyed object later.
        prepareExpandedExpression(
            const_cast<AssertionResult&>( assertionStats.assertionResult ) );
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
        auto node = std::make_shared<TestCaseNode>( testCaseStats );
        assert( m_sectionStack.size() == 0 );
        node->children.push_back( m_rootSection );
        m_testCases.push_back( node );
        m_rootSection.reset();

        assert( m_deepestSection );
        m_deepestSection->stdOut = testCaseStats.stdOut;
        m_deepestSection->stdErr = testCaseStats.stdErr;
    }

    void CumulativeReporterBase::testGroupEnded(
        TestGroupStats const& testGroupStats ) {
        auto node = std::make_shared<TestGroupNode>( testGroupStats );
        node->children.swap( m_testCases );
        m_testGroups.push_back( node );
    }

    void CumulativeReporterBase::testRunEnded( TestRunStats const& testRunStats ) {
        auto node = std::make_shared<TestRunNode>( testRunStats );
        node->children.swap( m_testGroups );
        m_testRuns.push_back( node );
        testRunEndedCumulative();
    }


    std::ostream& operator<<(std::ostream& out, lineOfChars value) {
        for (size_t idx = 0; idx < CATCH_CONFIG_CONSOLE_WIDTH - 1; ++idx) {
            out.put(value.c);
        }
        return out;
    }

} // end namespace Catch
