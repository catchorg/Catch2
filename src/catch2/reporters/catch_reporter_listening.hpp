/*
 *  Created by Martin on 19/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MULTI_REPORTER_H_INCLUDED
#define TWOBLUECUBES_CATCH_MULTI_REPORTER_H_INCLUDED

#include <catch2/catch_interfaces_reporter.h>

namespace Catch {

    class ListeningReporter final : public IStreamingReporter {
        using Reporters = std::vector<IStreamingReporterPtr>;
        Reporters m_listeners;
        IStreamingReporterPtr m_reporter = nullptr;
        ReporterPreferences m_preferences;

    public:
        ListeningReporter();

        void addListener( IStreamingReporterPtr&& listener );
        void addReporter( IStreamingReporterPtr&& reporter );

    public: // IStreamingReporter

        ReporterPreferences getPreferences() const override;

        void noMatchingTestCases( std::string const& spec ) override;

        void reportInvalidArguments(std::string const&arg) override;

        void benchmarkPreparing(std::string const& name) override;
        void benchmarkStarting( BenchmarkInfo const& benchmarkInfo ) override;
        void benchmarkEnded( BenchmarkStats<> const& benchmarkStats ) override;
        void benchmarkFailed(std::string const&) override;

        void testRunStarting( TestRunInfo const& testRunInfo ) override;
        void testGroupStarting( GroupInfo const& groupInfo ) override;
        void testCaseStarting( TestCaseInfo const& testInfo ) override;
        void sectionStarting( SectionInfo const& sectionInfo ) override;
        void assertionStarting( AssertionInfo const& assertionInfo ) override;

        // The return value indicates if the messages buffer should be cleared:
        bool assertionEnded( AssertionStats const& assertionStats ) override;
        void sectionEnded( SectionStats const& sectionStats ) override;
        void testCaseEnded( TestCaseStats const& testCaseStats ) override;
        void testGroupEnded( TestGroupStats const& testGroupStats ) override;
        void testRunEnded( TestRunStats const& testRunStats ) override;

        void skipTest( TestCaseInfo const& testInfo ) override;

        void listReporters(std::vector<ReporterDescription> const& descriptions, Config const& config) override;
        void listTests(std::vector<TestCaseHandle> const& tests, Config const& config) override;
        void listTags(std::vector<TagInfo> const& tags, Config const& config) override;


    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_MULTI_REPORTER_H_INCLUDED