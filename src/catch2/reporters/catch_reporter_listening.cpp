
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/reporters/catch_reporter_listening.hpp>

#include <cassert>

namespace Catch {

    void ListeningReporter::addListener( IStreamingReporterPtr&& listener ) {
        m_listeners.push_back( std::move( listener ) );
    }

    void ListeningReporter::addReporter(IStreamingReporterPtr&& reporter) {
        assert(!m_reporter && "Listening reporter can wrap only 1 real reporter");
        m_reporter = std::move( reporter );
        m_preferences.shouldRedirectStdOut = m_reporter->getPreferences().shouldRedirectStdOut;
    }

    void ListeningReporter::noMatchingTestCases( std::string const& spec ) {
        for ( auto const& listener : m_listeners ) {
            listener->noMatchingTestCases( spec );
        }
        m_reporter->noMatchingTestCases( spec );
    }

    void ListeningReporter::reportInvalidArguments(std::string const&arg){
        for ( auto const& listener : m_listeners ) {
            listener->reportInvalidArguments( arg );
        }
        m_reporter->reportInvalidArguments( arg );
    }

    void ListeningReporter::benchmarkPreparing( std::string const& name ) {
        for (auto const& listener : m_listeners) {
            listener->benchmarkPreparing(name);
        }
        m_reporter->benchmarkPreparing(name);
    }
    void ListeningReporter::benchmarkStarting( BenchmarkInfo const& benchmarkInfo ) {
        for ( auto const& listener : m_listeners ) {
            listener->benchmarkStarting( benchmarkInfo );
        }
        m_reporter->benchmarkStarting( benchmarkInfo );
    }
    void ListeningReporter::benchmarkEnded( BenchmarkStats<> const& benchmarkStats ) {
        for ( auto const& listener : m_listeners ) {
            listener->benchmarkEnded( benchmarkStats );
        }
        m_reporter->benchmarkEnded( benchmarkStats );
    }

    void ListeningReporter::benchmarkFailed( std::string const& error ) {
        for (auto const& listener : m_listeners) {
            listener->benchmarkFailed(error);
        }
        m_reporter->benchmarkFailed(error);
    }

    void ListeningReporter::testRunStarting( TestRunInfo const& testRunInfo ) {
        for ( auto const& listener : m_listeners ) {
            listener->testRunStarting( testRunInfo );
        }
        m_reporter->testRunStarting( testRunInfo );
    }

    void ListeningReporter::testGroupStarting( GroupInfo const& groupInfo ) {
        for ( auto const& listener : m_listeners ) {
            listener->testGroupStarting( groupInfo );
        }
        m_reporter->testGroupStarting( groupInfo );
    }


    void ListeningReporter::testCaseStarting( TestCaseInfo const& testInfo ) {
        for ( auto const& listener : m_listeners ) {
            listener->testCaseStarting( testInfo );
        }
        m_reporter->testCaseStarting( testInfo );
    }

    void ListeningReporter::sectionStarting( SectionInfo const& sectionInfo ) {
        for ( auto const& listener : m_listeners ) {
            listener->sectionStarting( sectionInfo );
        }
        m_reporter->sectionStarting( sectionInfo );
    }

    void ListeningReporter::assertionStarting( AssertionInfo const& assertionInfo ) {
        for ( auto const& listener : m_listeners ) {
            listener->assertionStarting( assertionInfo );
        }
        m_reporter->assertionStarting( assertionInfo );
    }

    // The return value indicates if the messages buffer should be cleared:
    bool ListeningReporter::assertionEnded( AssertionStats const& assertionStats ) {
        for( auto const& listener : m_listeners ) {
            static_cast<void>( listener->assertionEnded( assertionStats ) );
        }
        return m_reporter->assertionEnded( assertionStats );
    }

    void ListeningReporter::sectionEnded( SectionStats const& sectionStats ) {
        for ( auto const& listener : m_listeners ) {
            listener->sectionEnded( sectionStats );
        }
        m_reporter->sectionEnded( sectionStats );
    }

    void ListeningReporter::testCaseEnded( TestCaseStats const& testCaseStats ) {
        for ( auto const& listener : m_listeners ) {
            listener->testCaseEnded( testCaseStats );
        }
        m_reporter->testCaseEnded( testCaseStats );
    }

    void ListeningReporter::testGroupEnded( TestGroupStats const& testGroupStats ) {
        for ( auto const& listener : m_listeners ) {
            listener->testGroupEnded( testGroupStats );
        }
        m_reporter->testGroupEnded( testGroupStats );
    }

    void ListeningReporter::testRunEnded( TestRunStats const& testRunStats ) {
        for ( auto const& listener : m_listeners ) {
            listener->testRunEnded( testRunStats );
        }
        m_reporter->testRunEnded( testRunStats );
    }


    void ListeningReporter::skipTest( TestCaseInfo const& testInfo ) {
        for ( auto const& listener : m_listeners ) {
            listener->skipTest( testInfo );
        }
        m_reporter->skipTest( testInfo );
    }

    void ListeningReporter::listReporters(std::vector<ReporterDescription> const& descriptions) {
        for (auto const& listener : m_listeners) {
            listener->listReporters(descriptions);
        }
        m_reporter->listReporters(descriptions);
    }

    void ListeningReporter::listTests(std::vector<TestCaseHandle> const& tests) {
        for (auto const& listener : m_listeners) {
            listener->listTests(tests);
        }
        m_reporter->listTests(tests);
    }

    void ListeningReporter::listTags(std::vector<TagInfo> const& tags) {
        for (auto const& listener : m_listeners) {
            listener->listTags(tags);
        }
        m_reporter->listTags(tags);
    }

} // end namespace Catch
