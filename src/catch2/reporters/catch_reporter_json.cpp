
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

namespace Catch {
    namespace {
        void writeSourceInfo( JsonObjectWriter& writer,
                              SourceLineInfo const& sourceInfo ) {
            auto source_location_writer =
                writer.write( "source-location" ).writeObject();
            source_location_writer.write( "filename" ).write( sourceInfo.file );
            source_location_writer.write( "line" ).write( sourceInfo.line );
        }

        void writeTags( JsonArrayWriter writer, std::vector<Tag> const& tags ) {
            for ( auto const& tag : tags ) {
                writer.write( tag.original );
            }
        }

        void writeProperties( JsonArrayWriter writer,
                              TestCaseInfo const& info ) {
            if ( info.isHidden() ) { writer.write( "is-hidden" ); }
            if ( info.okToFail() ) { writer.write( "ok-to-fail" ); }
            if ( info.expectedToFail() ) { writer.write( "expected-to-fail" ); }
            if ( info.throws() ) { writer.write( "throws" ); }
        }

        void writeCounts( JsonObjectWriter writer, Counts const& counts ) {
            writer.write( "passed" ).write( counts.passed );
            writer.write( "failed" ).write( counts.failed );
            writer.write( "fail-but-ok" ).write( counts.failedButOk );
            writer.write( "skipped" ).write( counts.skipped );
        }

        void writeTestInfo( JsonObjectWriter writer,
                            TestCaseInfo const& info ) {
            writer.write( "name" ).write( info.name );
            writeTags( writer.write( "tags" ).writeArray(), info.tags );
            writeSourceInfo( writer, info.lineInfo );
            writeProperties( writer.write( "properties" ).writeArray(), info );
        }

        void writeSection( JsonObjectWriter& writer,
                           CumulativeReporterBase::SectionNode const& section,
                           bool selfWrite ) {
            if ( selfWrite ) {
                writer.write( "name" ).write( section.stats.sectionInfo.name );
                writeSourceInfo( writer, section.stats.sectionInfo.lineInfo );
                writeCounts( writer.write( "assertions-stats" ).writeObject(),
                             section.stats.assertions );
            }
            if ( section.childSections.empty() ) { return; }
            auto sectionsWriter = writer.write( "sections" ).writeArray();
            for ( auto const& childPtr : section.childSections ) {
                auto childSectionWriter = sectionsWriter.writeObject();
                writeSection( childSectionWriter, *childPtr, true );
            }
        }
    } // namespace
    JsonReporter::JsonReporter( ReporterConfig&& config ):
        CumulativeReporterBase{ CATCH_MOVE( config ) } {
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

    JsonArrayWriter& JsonReporter::startArray() {
        m_arrayWriters.emplace( m_arrayWriters.top().writeArray() );
        m_writers.emplace( Writer::Array );
        return m_arrayWriters.top();
    }
    JsonArrayWriter& JsonReporter::startArray( std::string const& key ) {
        m_arrayWriters.emplace(
            m_objectWriters.top().write( key ).writeArray() );
        m_writers.emplace( Writer::Array );
        return m_arrayWriters.top();
    }

    JsonObjectWriter& JsonReporter::startObject() {
        m_objectWriters.emplace( m_arrayWriters.top().writeObject() );
        m_writers.emplace( Writer::Object );
        return m_objectWriters.top();
    }
    JsonObjectWriter& JsonReporter::startObject( std::string const& key ) {
        m_objectWriters.emplace(
            m_objectWriters.top().write( key ).writeObject() );
        m_writers.emplace( Writer::Object );
        return m_objectWriters.top();
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
        CumulativeReporterBase::testRunStarting( testInfo );

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

        startArray( "test-cases" );
    }

    void JsonReporter::testRunEndedCumulative() {
        for ( auto const& testCase : m_testRun->children ) {
            assert( testCase->children.size() == 1 );

            auto writer = m_arrayWriters.top().writeObject();

            writeTestInfo( writer.write( "test-info" ).writeObject(),
                           *testCase->value.testInfo );
            writeCounts( writer.write( "totals" ).writeObject(),
                         testCase->value.totals.assertions );
            writeSection( writer, *testCase->children.front(), false );
        }
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
            writeSourceInfo( desc_writer, info.lineInfo );
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
