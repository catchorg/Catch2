
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_CMAKE_HPP_INCLUDED
#define CATCH_REPORTER_CMAKE_HPP_INCLUDED

#include <catch2/reporters/catch_reporter_streaming_base.hpp>

namespace Catch {

    class CMakeReporter final : public StreamingReporterBase {
    public:
        using StreamingReporterBase::StreamingReporterBase;

        static std::string getDescription();

        void listReporters(
            std::vector<ReporterDescription> const& descriptions ) override;
        void listListeners(
            std::vector<ListenerDescription> const& descriptions ) override;
        void listTests( std::vector<TestCaseHandle> const& tests ) override;
        void listTags( std::vector<TagInfo> const& tags ) override;
    };

} // end namespace Catch

#endif // CATCH_REPORTER_CMAKE_HPP_INCLUDED
