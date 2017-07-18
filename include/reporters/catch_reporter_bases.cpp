/*
 *  Created by Phil on 27/11/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "../internal/catch_interfaces_reporter.h"
#include "../internal/catch_errno_guard.h"
#include "catch_reporter_bases.hpp"

#include <cstring>
#include <cfloat>
#include <cstdio>
#include <assert.h>
#include <memory>

namespace Catch {

    // Because formatting using c++ streams is stateful, drop down to C is required
    // Alternatively we could use stringstream, but its performance is... not good.
    std::string getFormattedDuration( double duration ) {
        // Max exponent + 1 is required to represent the whole part
        // + 1 for decimal point
        // + 3 for the 3 decimal places
        // + 1 for null terminator
        const size_t maxDoubleSize = DBL_MAX_10_EXP + 1 + 1 + 3 + 1;
        char buffer[maxDoubleSize];
 
        // Save previous errno, to prevent sprintf from overwriting it
        ErrnoGuard guard;
#ifdef _MSC_VER
        sprintf_s(buffer, "%.3f", duration);
#else
        sprintf(buffer, "%.3f", duration);
#endif
        return std::string(buffer);
    }


    StreamingReporterBase::StreamingReporterBase(ReporterConfig const& _config)
    :   m_config(_config.fullConfig()),
        stream(_config.stream())
    {
        m_reporterPrefs.shouldRedirectStdOut = false;
    }

    ReporterPreferences StreamingReporterBase::getPreferences() const {
        return m_reporterPrefs;
    }

    StreamingReporterBase::~StreamingReporterBase() {}

    void StreamingReporterBase::noMatchingTestCases(std::string const &) {}

    void StreamingReporterBase::testRunStarting(TestRunInfo const & _testRunInfo) {
        currentTestRunInfo = _testRunInfo;
    }

    void StreamingReporterBase::testGroupStarting(GroupInfo const & _groupInfo) {
        currentGroupInfo = _groupInfo;
    }

    void StreamingReporterBase::testCaseStarting(TestCaseInfo const & _testInfo) {
        currentTestCaseInfo = _testInfo;
    }

    void StreamingReporterBase::sectionStarting(SectionInfo const & _sectionInfo) {
        m_sectionStack.push_back(_sectionInfo);
    }

    void StreamingReporterBase::sectionEnded(SectionStats const &) {
        m_sectionStack.pop_back();
    }

    void StreamingReporterBase::testCaseEnded(TestCaseStats const &) {
        currentTestCaseInfo.reset();
    }

    void StreamingReporterBase::testGroupEnded(TestGroupStats const &) {
        currentGroupInfo.reset();
    }

    void StreamingReporterBase::testRunEnded(TestRunStats const &) {
        currentTestCaseInfo.reset();
        currentGroupInfo.reset();
        currentTestRunInfo.reset();
    }

    void StreamingReporterBase::skipTest(TestCaseInfo const &) {
        // Don't do anything with this by default.
        // It can optionally be overridden in the derived class.
    }

    CumulativeReporterBase::CumulativeReporterBase(ReporterConfig const & _config)
        : m_config(_config.fullConfig()),
        stream(_config.stream()) {
        m_reporterPrefs.shouldRedirectStdOut = false;
    }

    CumulativeReporterBase::~CumulativeReporterBase() {}

    ReporterPreferences CumulativeReporterBase::getPreferences() const {
        return m_reporterPrefs;
    }

    void CumulativeReporterBase::testRunStarting(TestRunInfo const &) {}

    void CumulativeReporterBase::testGroupStarting(GroupInfo const &) {}

    void CumulativeReporterBase::testCaseStarting(TestCaseInfo const &) {}

    void CumulativeReporterBase::sectionStarting(SectionInfo const & sectionInfo) {
        SectionStats incompleteStats(sectionInfo, Counts(), 0, false);
        std::shared_ptr<SectionNode> node;
        if (m_sectionStack.empty()) {
            if (!m_rootSection)
                m_rootSection = std::make_shared<SectionNode>(incompleteStats);
            node = m_rootSection;
        } else {
            SectionNode& parentNode = *m_sectionStack.back();
            auto it =
                std::find_if(parentNode.childSections.begin(),
                             parentNode.childSections.end(),
                             BySectionInfo(sectionInfo));
            if (it == parentNode.childSections.end()) {
                node = std::make_shared<SectionNode>(incompleteStats);
                parentNode.childSections.push_back(node);
            } else
                node = *it;
        }
        m_sectionStack.push_back(node);
        m_deepestSection = node;
    }

    void CumulativeReporterBase::assertionStarting(AssertionInfo const &) {}

    bool CumulativeReporterBase::assertionEnded(AssertionStats const & assertionStats) {
        assert(!m_sectionStack.empty());
        SectionNode& sectionNode = *m_sectionStack.back();
        sectionNode.assertions.push_back(assertionStats);
        // AssertionResult holds a pointer to a temporary DecomposedExpression,
        // which getExpandedExpression() calls to build the expression string.
        // Our section stack copy of the assertionResult will likely outlive the
        // temporary, so it must be expanded or discarded now to avoid calling
        // a destroyed object later.
        prepareExpandedExpression(sectionNode.assertions.back().assertionResult);
        return true;
    }

    void CumulativeReporterBase::sectionEnded(SectionStats const & sectionStats) {
        assert(!m_sectionStack.empty());
        SectionNode& node = *m_sectionStack.back();
        node.stats = sectionStats;
        m_sectionStack.pop_back();
    }

    void CumulativeReporterBase::testCaseEnded(TestCaseStats const & testCaseStats) {
        auto node = std::make_shared<TestCaseNode>(testCaseStats);
        assert(m_sectionStack.size() == 0);
        node->children.push_back(m_rootSection);
        m_testCases.push_back(node);
        m_rootSection.reset();

        assert(m_deepestSection);
        m_deepestSection->stdOut = testCaseStats.stdOut;
        m_deepestSection->stdErr = testCaseStats.stdErr;
    }

    void CumulativeReporterBase::testGroupEnded(TestGroupStats const & testGroupStats) {
        auto node = std::make_shared<TestGroupNode>(testGroupStats);
        node->children.swap(m_testCases);
        m_testGroups.push_back(node);
    }

    void CumulativeReporterBase::testRunEnded(TestRunStats const & testRunStats) {
        auto node = std::make_shared<TestRunNode>(testRunStats);
        node->children.swap(m_testGroups);
        m_testRuns.push_back(node);
        testRunEndedCumulative();
    }

    void CumulativeReporterBase::skipTest(TestCaseInfo const &) {}

    void CumulativeReporterBase::prepareExpandedExpression(AssertionResult & result) const {
        if (result.isOk())
            result.discardDecomposedExpression();
        else
            result.expandDecomposedExpression();
    }

    CumulativeReporterBase::SectionNode::SectionNode(SectionStats const& _stats)
        :stats(_stats) {}
    CumulativeReporterBase::SectionNode::~SectionNode() {}

    bool CumulativeReporterBase::SectionNode::operator==(SectionNode const & other) const {
        return stats.sectionInfo.lineInfo == other.stats.sectionInfo.lineInfo;
    }

    bool CumulativeReporterBase::SectionNode::operator==(std::shared_ptr<SectionNode> const& other) const {
        return operator==(*other);
    }


    CumulativeReporterBase::BySectionInfo::BySectionInfo(SectionInfo const& other)
        :m_other(other) {}

    CumulativeReporterBase::BySectionInfo::BySectionInfo(BySectionInfo const & other)
        :m_other(other.m_other) {}

    bool CumulativeReporterBase::BySectionInfo::operator()(std::shared_ptr<SectionNode> const & node) const {
        return node->stats.sectionInfo.lineInfo == m_other.lineInfo;
    }


    TestEventListenerBase::TestEventListenerBase(ReporterConfig const & _config)
        :StreamingReporterBase(_config) {}

    void TestEventListenerBase::assertionStarting(AssertionInfo const &) {}

    bool TestEventListenerBase::assertionEnded(AssertionStats const &) {
        return false;
    }


} // end namespace Catch
