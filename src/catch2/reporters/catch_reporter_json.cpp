
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
#include <catch2/internal/catch_list.hpp>
#include <catch2/internal/catch_string_manip.hpp>
#include <catch2/reporters/catch_reporter_json.hpp>

namespace {
    using namespace Catch;

    void writeSourceInfo( JsonObjectWriter& writer,
                          SourceLineInfo const& sourceInfo ) {
        writer.write( "filename" ).write( sourceInfo.file );
        writer.write( "line" ).write( sourceInfo.line );
    }
    void writeSourceInfo( JsonObjectWriter&& writer,
                          SourceLineInfo const& sourceInfo ) {
        writeSourceInfo( writer, sourceInfo );
    }

    void writeCounts( JsonObjectWriter writer, Counts const& counts ) {
        writer.write( "passed" ).write( counts.passed );
        writer.write( "failed" ).write( counts.failed );
        writer.write( "fail-but-ok" ).write( counts.failedButOk );
        writer.write( "skipped" ).write( counts.skipped );
    }

} // namespace

namespace Catch {
    JsonReporter::JsonReporter( ReporterConfig&& config ):
        StreamingReporterBase{ CATCH_MOVE( config ) } {
        m_objectWriters.emplace( m_stream );
        m_writers.emplace( Writer::Object );
    }

    JsonReporter::~JsonReporter() {
        while ( !m_writers.empty() ) {
            switch ( m_writers.top() ) {
            case Writer::Object:
                endObject();
                break;
            case Writer::Array:
                endArray();
                break;
            }
        }
    }

    void JsonReporter::startArray() {
        if ( !isInside( Writer::Array ) ) { return; }
        m_arrayWriters.emplace( m_arrayWriters.top().writeArray() );
        m_writers.emplace( Writer::Array );
    }
    void JsonReporter::startArray( std::string const& key ) {
        if ( !isInside( Writer::Object ) ) { return; }
        m_arrayWriters.emplace(
            m_objectWriters.top().write( key ).writeArray() );
        m_writers.emplace( Writer::Array );
    }

    void JsonReporter::startObject() {
        if ( !isInside( Writer::Array ) ) { return; }
        m_objectWriters.emplace( m_arrayWriters.top().writeObject() );
        m_writers.emplace( Writer::Object );
    }
    void JsonReporter::startObject( std::string const& key ) {
        if ( !isInside( Writer::Object ) ) { return; }
        m_objectWriters.emplace(
            m_objectWriters.top().write( key ).writeObject() );
        m_writers.emplace( Writer::Object );
    }

    void JsonReporter::endObject() {
        if ( !isInside( Writer::Object ) ) { return; }
        m_objectWriters.pop();
        m_writers.pop();
    }
    void JsonReporter::endArray() {
        if ( !isInside( Writer::Array ) ) { return; }
        m_arrayWriters.pop();
        m_writers.pop();
    }

    bool JsonReporter::isInside( Writer writer ) {
        return !m_writers.empty() && m_writers.top() == writer;
    }

    std::string JsonReporter::getDescription() {
        return "Reports test results as a JSON document";
    }

    void JsonReporter::testRunStarting( TestRunInfo const& testInfo ) {
        StreamingReporterBase::testRunStarting( testInfo );

        if ( !isInside( Writer::Object ) ) { return; }

        auto& writer = m_objectWriters.top();

        writer.write( "version" ).write( 1 );

        {
            auto metadata_writer = writer.write( "metadata" ).writeObject();
            metadata_writer.write( "name" ).write( m_config->name() );
            metadata_writer.write( "rng-seed" ).write( m_config->rngSeed() );
            metadata_writer.write( "catch2-version" ).write( libraryVersion() );
            if ( m_config->testSpec().hasFilters() ) {
                metadata_writer.write( "filters" )
                    .write( m_config->testSpec() );
            }
        }

        startArray( "test-results" );
    }

    void JsonReporter::testCaseStarting( TestCaseInfo const& testInfo ) {
        StreamingReporterBase::testCaseStarting( testInfo );

        if ( !isInside( Writer::Array ) ) { return; }

        startObject();

        auto& writer = m_objectWriters.top();
        writer.write( "name" ).write( trim( StringRef( testInfo.name ) ) );

        {
            auto array_writer = writer.write( "tags" ).writeArray();
            for ( const auto& tag : testInfo.tags ) {
                array_writer.write( tag.original );
            }
        }

        writeSourceInfo( writer, testInfo.lineInfo );

        if ( m_config->showDurations() == ShowDurations::Always ) {
            m_testCaseTimer.start();
        }
        startArray( "sections" );
    }

    void JsonReporter::sectionStarting( SectionInfo const& sectionInfo ) {
        StreamingReporterBase::sectionStarting( sectionInfo );
    }

    void JsonReporter::assertionStarting( AssertionInfo const& ) {}

    void JsonReporter::assertionEnded( AssertionStats const& assertionStats ) {
        (void)assertionStats;
    }

    void JsonReporter::sectionEnded( SectionStats const& sectionStats ) {
        StreamingReporterBase::sectionEnded( sectionStats );

        if ( !isInside( Writer::Array ) ) { return; }

        auto writer = m_arrayWriters.top().writeObject();
        writer.write( "name" ).write( sectionStats.sectionInfo.name );
        writeSourceInfo( writer, sectionStats.sectionInfo.lineInfo );
        writeCounts( writer.write( "assertions" ).writeObject(),
                     sectionStats.assertions );
    }

    void JsonReporter::testCaseEnded( TestCaseStats const& testCaseStats ) {
        StreamingReporterBase::testCaseEnded( testCaseStats );

        if ( !isInside( Writer::Array ) ) { return; }

        endArray();

        if ( !isInside( Writer::Object ) ) { return; }

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

            auto totals_writer = writer.write( "totals" ).writeObject();
            writeCounts( totals_writer.write( "assertions" ).writeObject(),
                         testCaseStats.totals.assertions );
            writeCounts( totals_writer.write( "test-cases" ).writeObject(),
                         testCaseStats.totals.testCases );
        }

        endObject();
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
        if ( !isInside( Writer::Object ) ) { return; }

        auto writer = m_objectWriters.top().write( "reporters" ).writeArray();
        for ( auto const& desc : descriptions ) {
            auto desc_writer = writer.writeObject();
            desc_writer.write( "name" ).write( desc.name );
            desc_writer.write( "description" ).write( desc.description );
        }
    }
    void JsonReporter::listListeners(
        std::vector<ListenerDescription> const& descriptions ) {
        if ( !isInside( Writer::Object ) ) { return; }

        auto writer = m_objectWriters.top().write( "listeners" ).writeArray();

        for ( auto const& desc : descriptions ) {
            auto desc_writer = writer.writeObject();
            desc_writer.write( "name" ).write( desc.name );
            desc_writer.write( "description" ).write( desc.description );
        }
    }
    void JsonReporter::listTests( std::vector<TestCaseHandle> const& tests ) {
        if ( !isInside( Writer::Object ) ) { return; }

        auto writer = m_objectWriters.top().write( "tests" ).writeArray();

        for ( auto const& test : tests ) {
            auto desc_writer = writer.writeObject();
            auto const& info = test.getTestCaseInfo();

            desc_writer.write( "name" ).write( info.name );
            desc_writer.write( "class-name" ).write( info.className );
            {
                auto tag_writer = desc_writer.write( "tags" ).writeArray();
                for ( auto const& tag : info.tags ) {
                    tag_writer.write( tag.original );
                }
            }
            writeSourceInfo( desc_writer.write( "source-info" ).writeObject(),
                             info.lineInfo );
        }
    }
    void JsonReporter::listTags( std::vector<TagInfo> const& tags ) {
        if ( !isInside( Writer::Object ) ) { return; }

        auto writer = m_objectWriters.top().write( "tags" ).writeArray();
        for ( auto const& tag : tags ) {
            auto tag_writer = writer.writeObject();
            {
                auto aliases_writer =
                    tag_writer.write( "aliases" ).writeArray();
                for ( auto alias : tag.spellings ) {
                    aliases_writer.write( alias );
                }
            }
            tag_writer.write( "count" ).write( tag.count );
        }
    }
} // namespace Catch
