
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/reporters/catch_reporter_listening.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>

#include <cassert>

namespace Catch {

    void ListeningReporter::addListener( IStreamingReporterPtr&& listener ) {
        m_listeners.push_back( CATCH_MOVE( listener ) );
    }

    void ListeningReporter::addReporter(IStreamingReporterPtr&& reporter) {
        assert(!m_reporter && "Listening reporter can wrap only 1 real reporter");
        m_reporter = CATCH_MOVE( reporter );
        m_preferences.shouldRedirectStdOut = m_reporter->getPreferences().shouldRedirectStdOut;
    }

    void ListeningReporter::noMatchingTestCases( StringRef unmatchedSpec ) {
        for ( auto& listener : m_listeners ) {
            listener->noMatchingTestCases( unmatchedSpec );
        }
        m_reporter->noMatchingTestCases( unmatchedSpec );
    }

    void ListeningReporter::fatalErrorEncountered( StringRef error ) {
        for ( auto& listener : m_listeners ) {
            listener->fatalErrorEncountered( error );
        }
        m_reporter->fatalErrorEncountered( error );
    }

    void ListeningReporter::reportInvalidTestSpec( StringRef arg ) {
        for ( auto& listener : m_listeners ) {
            listener->reportInvalidTestSpec( arg );
        }
        m_reporter->reportInvalidTestSpec( arg );
    }

    void ListeningReporter::benchmarkPreparing( StringRef name ) {
        for (auto& listener : m_listeners) {
            listener->benchmarkPreparing(name);
        }
        m_reporter->benchmarkPreparing(name);
    }
    void ListeningReporter::benchmarkStarting( BenchmarkInfo const& benchmarkInfo ) {
        for ( auto& listener : m_listeners ) {
            listener->benchmarkStarting( benchmarkInfo );
        }
        m_reporter->benchmarkStarting( benchmarkInfo );
    }
    void ListeningReporter::benchmarkEnded( BenchmarkStats<> const& benchmarkStats ) {
        for ( auto& listener : m_listeners ) {
            listener->benchmarkEnded( benchmarkStats );
        }
        m_reporter->benchmarkEnded( benchmarkStats );
    }

    void ListeningReporter::benchmarkFailed( StringRef error ) {
        for (auto& listener : m_listeners) {
            listener->benchmarkFailed(error);
        }
        m_reporter->benchmarkFailed(error);
    }

    void ListeningReporter::testRunStarting( TestRunInfo const& testRunInfo ) {
        for ( auto& listener : m_listeners ) {
            listener->testRunStarting( testRunInfo );
        }
        m_reporter->testRunStarting( testRunInfo );
    }

    void ListeningReporter::testCaseStarting( TestCaseInfo const& testInfo ) {
        for ( auto& listener : m_listeners ) {
            listener->testCaseStarting( testInfo );
        }
        m_reporter->testCaseStarting( testInfo );
    }

    void
    ListeningReporter::testCasePartialStarting( TestCaseInfo const& testInfo,
                                                uint64_t partNumber ) {
        for ( auto& listener : m_listeners ) {
            listener->testCasePartialStarting( testInfo, partNumber );
        }
        m_reporter->testCasePartialStarting( testInfo, partNumber );
    }

    void ListeningReporter::sectionStarting( SectionInfo const& sectionInfo ) {
        for ( auto& listener : m_listeners ) {
            listener->sectionStarting( sectionInfo );
        }
        m_reporter->sectionStarting( sectionInfo );
    }

    void ListeningReporter::assertionStarting( AssertionInfo const& assertionInfo ) {
        for ( auto& listener : m_listeners ) {
            listener->assertionStarting( assertionInfo );
        }
        m_reporter->assertionStarting( assertionInfo );
    }

    // The return value indicates if the messages buffer should be cleared:
    void ListeningReporter::assertionEnded( AssertionStats const& assertionStats ) {
        for( auto& listener : m_listeners ) {
            listener->assertionEnded( assertionStats );
        }
        m_reporter->assertionEnded( assertionStats );
    }

    void ListeningReporter::sectionEnded( SectionStats const& sectionStats ) {
        for ( auto& listener : m_listeners ) {
            listener->sectionEnded( sectionStats );
        }
        m_reporter->sectionEnded( sectionStats );
    }

    void ListeningReporter::testCasePartialEnded( TestCaseStats const& testInfo,
                                                  uint64_t partNumber ) {
        for ( auto& listener : m_listeners ) {
            listener->testCasePartialEnded( testInfo, partNumber );
        }
        m_reporter->testCasePartialEnded( testInfo, partNumber );
    }

    void ListeningReporter::testCaseEnded( TestCaseStats const& testCaseStats ) {
        for ( auto& listener : m_listeners ) {
            listener->testCaseEnded( testCaseStats );
        }
        m_reporter->testCaseEnded( testCaseStats );
    }

    void ListeningReporter::testRunEnded( TestRunStats const& testRunStats ) {
        for ( auto& listener : m_listeners ) {
            listener->testRunEnded( testRunStats );
        }
        m_reporter->testRunEnded( testRunStats );
    }


    void ListeningReporter::skipTest( TestCaseInfo const& testInfo ) {
        for ( auto& listener : m_listeners ) {
            listener->skipTest( testInfo );
        }
        m_reporter->skipTest( testInfo );
    }

    void ListeningReporter::listReporters(std::vector<ReporterDescription> const& descriptions) {
        for (auto& listener : m_listeners) {
            listener->listReporters(descriptions);
        }
        m_reporter->listReporters(descriptions);
    }

    void ListeningReporter::listTests(std::vector<TestCaseHandle> const& tests) {
        for (auto& listener : m_listeners) {
            listener->listTests(tests);
        }
        m_reporter->listTests(tests);
    }

    void ListeningReporter::listTags(std::vector<TagInfo> const& tags) {
        for (auto& listener : m_listeners) {
            listener->listTags(tags);
        }
        m_reporter->listTags(tags);
    }

} // end namespace Catch
