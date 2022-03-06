
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_COMMON_BASE_HPP_INCLUDED
#define CATCH_REPORTER_COMMON_BASE_HPP_INCLUDED

#include <catch2/interfaces/catch_interfaces_reporter.hpp>

namespace Catch {
    /**
     * This is the base class for all reporters.
     *
     * If are writing a reporter, you must derive from this type, or one
     * of the helper reporter bases that are derived from this type.
     *
     * ReporterBase centralizes handling of various common tasks in reporters,
     * like storing the right stream for the reporters to write to, and
     * providing the default implementation of the different listing events.
     */
    class ReporterBase : public IEventListener {
    protected:
        //! Stream to write the output to
        std::ostream& m_stream;

    public:
        ReporterBase( ReporterConfig const& config ):
            IEventListener( config.fullConfig() ),
            m_stream( config.stream() ) {}


        /**
         * Provides a simple default listing of reporters.
         *
         * Should look roughly like the reporter listing in v2 and earlier
         * versions of Catch2.
         */
        void listReporters(
            std::vector<ReporterDescription> const& descriptions ) override;
        /**
         * Provides a simple default listing of tests.
         *
         * Should look roughly like the test listing in v2 and earlier versions
         * of Catch2. Especially supports low-verbosity listing that mimics the
         * old `--list-test-names-only` output.
         */
        void listTests( std::vector<TestCaseHandle> const& tests ) override;
        /**
         * Provides a simple default listing of tags.
         *
         * Should look roughly like the tag listing in v2 and earlier versions
         * of Catch2.
         */
        void listTags( std::vector<TagInfo> const& tags ) override;
    };
} // namespace Catch

#endif // CATCH_REPORTER_COMMON_BASE_HPP_INCLUDED
