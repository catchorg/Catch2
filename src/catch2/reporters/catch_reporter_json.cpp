
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
//
#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_test_spec.hpp>
#include <catch2/catch_version.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>
#include <catch2/internal/catch_string_manip.hpp>
#include <catch2/reporters/catch_reporter_json.hpp>

namespace Catch {
    JsonReporter::JsonReporter( ReporterConfig&& config ):
        StreamingReporterBase{ CATCH_MOVE( config ) } {
        m_objectWriters.emplace( m_stream );
        m_writers.emplace( Writer::Object );
    }

    JsonReporter::~JsonReporter() = default;

    void JsonReporter::pushArray() {
        if ( !isInside( Writer::Array ) ) { return; }
        m_arrayWriters.emplace( m_arrayWriters.top().writeArray() );
        m_writers.emplace( Writer::Array );
    }
    void JsonReporter::pushArray( std::string const& key ) {
        if ( !isInside( Writer::Object ) ) { return; }
        m_arrayWriters.emplace(
            m_objectWriters.top().write( key ).writeArray() );
        m_writers.emplace( Writer::Array );
    }

    void JsonReporter::pushObject() {
        if ( !isInside( Writer::Array ) ) { return; }
        m_objectWriters.emplace( m_arrayWriters.top().writeObject() );
        m_writers.emplace( Writer::Object );
    }
    void JsonReporter::pushObject( std::string const& key ) {
        if ( !isInside( Writer::Object ) ) { return; }
        m_objectWriters.emplace(
            m_objectWriters.top().write( key ).writeObject() );
        m_writers.emplace( Writer::Object );
    }

    bool JsonReporter::isInside( Writer writer ) {
        return not m_writers.empty() && m_writers.top() == writer;
    }

    std::string JsonReporter::getDescription() {
        return "Reports test results as a JSON document";
    }

    void JsonReporter::writeSourceInfo( SourceLineInfo const& sourceInfo ) {
        if ( !isInside( Writer::Object ) ) { return; }

        auto& writer = m_objectWriters.top();
        writer.write( "filename" ).write( sourceInfo.file );
        writer.write( "line" ).write( sourceInfo.line );
    }

    void JsonReporter::testRunStarting( TestRunInfo const& testInfo ) {
        if ( !isInside( Writer::Object ) ) { return; }

        StreamingReporterBase::testRunStarting( testInfo );

        auto& writer = m_objectWriters.top();

        writer.write( "name" ).write( m_config->name() );
        writer.write( "rng-seed" ).write( m_config->rngSeed() );
        writer.write( "catch2-version" ).write( libraryVersion() );

        if ( m_config->testSpec().hasFilters() ) {
            writer.write( "filters" ).write( m_config->testSpec() );
        }

        pushArray( "test-cases" );
    }

    void JsonReporter::testCaseStarting( TestCaseInfo const& testInfo ) {
        if ( !isInside( Writer::Array ) ) { return; }

        StreamingReporterBase::testCaseStarting( testInfo );

        pushObject();

        auto& writer = m_objectWriters.top();
        writer.write( "name" ).write( trim( StringRef( testInfo.name ) ) );

        {
            auto array_writer = writer.write( "tags" ).writeArray();
            for ( const auto& tag : testInfo.tags ) {
                array_writer.write( tag.original );
            }
        }

        writeSourceInfo( testInfo.lineInfo );

        if ( m_config->showDurations() == ShowDurations::Always ) {
            m_testCaseTimer.start();
        }
    }

    void JsonReporter::sectionStarting( SectionInfo const& sectionInfo ) {
        (void)sectionInfo;
    }

    void JsonReporter::assertionStarting( AssertionInfo const& ) {}

    void JsonReporter::assertionEnded( AssertionStats const& assertionStats ) {
        (void)assertionStats;
    }

    void JsonReporter::sectionEnded( SectionStats const& sectionStats ) {
        (void)sectionStats;
    }

    void JsonReporter::testCaseEnded( TestCaseStats const& testCaseStats ) {
        if ( !isInside( Writer::Object ) ) { return; }

        StreamingReporterBase::testCaseEnded( testCaseStats );

        if ( m_config->showDurations() == ShowDurations::Always ) {
            // TODO: Handle this
        }

        auto& writer = m_objectWriters.top();
        writer.write( "is-hidden" ).write( testCaseStats.testInfo->isHidden() );
        writer.write( "throws" ).write( testCaseStats.testInfo->throws() );
        writer.write( "ok-to-fail" )
            .write( testCaseStats.testInfo->okToFail() );
        writer.write( "expected-to-fail" )
            .write( testCaseStats.testInfo->expectedToFail() );
        {
            auto totalsWriter = writer.write( "totals" ).writeObject();
            {
                auto assertionsWriter =
                    totalsWriter.write( "assertions" ).writeObject();
                assertionsWriter.write( "passed" )
                    .write( testCaseStats.totals.assertions.passed );
                assertionsWriter.write( "failed" )
                    .write( testCaseStats.totals.assertions.failed );
                assertionsWriter.write( "fail-but-ok" )
                    .write( testCaseStats.totals.assertions.failedButOk );
                assertionsWriter.write( "skipped" )
                    .write( testCaseStats.totals.assertions.skipped );
            }
            {
                auto testsCasesWriter =
                    totalsWriter.write( "test-cases" ).writeObject();
                testsCasesWriter.write( "passed" )
                    .write( testCaseStats.totals.testCases.passed );
                testsCasesWriter.write( "failed" )
                    .write( testCaseStats.totals.testCases.failed );
                testsCasesWriter.write( "fail-but-ok" )
                    .write( testCaseStats.totals.testCases.failedButOk );
                testsCasesWriter.write( "skipped" )
                    .write( testCaseStats.totals.testCases.skipped );
            }
        }

        m_objectWriters.pop();
        m_writers.pop();
    }

    void JsonReporter::testRunEnded( TestRunStats const& testRunStats ) {
        (void)testRunStats;
    }

    void JsonReporter::benchmarkPreparing( StringRef name ) { (void)name; }
    void JsonReporter::benchmarkStarting( BenchmarkInfo const& ) {}
    void JsonReporter::benchmarkEnded( BenchmarkStats<> const& ) {}
    void JsonReporter::benchmarkFailed( StringRef error ) { (void)error; }

    void JsonReporter::listReporters(
        std::vector<ReporterDescription> const& descriptions ) {
        (void)descriptions;
    }
    void JsonReporter::listListeners(
        std::vector<ListenerDescription> const& descriptions ) {
        (void)descriptions;
    }
    void JsonReporter::listTests( std::vector<TestCaseHandle> const& tests ) {
        (void)tests;
    }
    void JsonReporter::listTags( std::vector<TagInfo> const& tags ) {
        (void)tags;
    }
} // namespace Catch
