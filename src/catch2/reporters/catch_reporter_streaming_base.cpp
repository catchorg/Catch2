
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/reporters/catch_reporter_streaming_base.hpp>
#include <catch2/reporters/catch_reporter_helpers.hpp>

namespace Catch {

    StreamingReporterBase::~StreamingReporterBase() = default;

    void
    StreamingReporterBase::testRunStarting( TestRunInfo const& _testRunInfo ) {
        currentTestRunInfo = _testRunInfo;
    }

    void StreamingReporterBase::testRunEnded( TestRunStats const& ) {
        currentTestCaseInfo = nullptr;
    }

    void StreamingReporterBase::listReporters(std::vector<ReporterDescription> const& descriptions) {
        defaultListReporters( m_stream, descriptions, m_config->verbosity() );
    }

    void StreamingReporterBase::listTests(std::vector<TestCaseHandle> const& tests) {
        defaultListTests(m_stream,
                         tests,
                         m_config->hasTestFilters(),
                         m_config->verbosity());
    }

    void StreamingReporterBase::listTags(std::vector<TagInfo> const& tags) {
        defaultListTags( m_stream, tags, m_config->hasTestFilters() );
    }

} // end namespace Catch
