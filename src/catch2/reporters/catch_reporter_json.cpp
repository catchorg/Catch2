
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

        //void writeCounts( JsonObjectWriter writer, Counts const& counts ) {
        //    writer.write( "passed" ).write( counts.passed );
        //    writer.write( "failed" ).write( counts.failed );
        //    writer.write( "fail-but-ok" ).write( counts.failedButOk );
        //    writer.write( "skipped" ).write( counts.skipped );
        //}

        //void writeTestInfo( JsonObjectWriter writer,
        //                    TestCaseInfo const& info ) {
        //    writer.write( "name" ).write( info.name );
        //    writeTags( writer.write( "tags" ).writeArray(), info.tags );
        //    writeSourceInfo( writer, info.lineInfo );
        //    writeProperties( writer.write( "properties" ).writeArray(), info );
        //}

        //void writeSection( JsonObjectWriter& writer,
        //                   CumulativeReporterBase::SectionNode const& section,
        //                   bool selfWrite ) {
        //    if ( selfWrite ) {
        //        writer.write( "name" ).write( section.stats.sectionInfo.name );
        //        writeSourceInfo( writer, section.stats.sectionInfo.lineInfo );
        //        writeCounts( writer.write( "assertions-stats" ).writeObject(),
        //                     section.stats.assertions );
        //    }
        //    if ( section.childSections.empty() ) { return; }
        //    auto sectionsWriter = writer.write( "sections" ).writeArray();
        //    for ( auto const& childPtr : section.childSections ) {
        //        auto childSectionWriter = sectionsWriter.writeObject();
        //        writeSection( childSectionWriter, *childPtr, true );
        //    }
        //}
    } // namespace

    JsonReporter::JsonReporter( ReporterConfig&& config ):
        StreamingReporterBase{ CATCH_MOVE( config ) } {

        m_preferences.shouldRedirectStdOut = true;
        // TBD: Do we want to report all assertions? XML reporter does
        //      not, but for machine-parseable reporters I think the answer
        //      should be yes.
        m_preferences.shouldReportAllAssertions = true;

        m_objectWriters.emplace( m_stream );
        m_writers.emplace( Writer::Object );
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
    }

    JsonReporter::~JsonReporter() {
        endListing();
        // TODO: Ensure this closes the top level object, add asserts
        assert( m_writers.size() == 1 && "Only the top level object should be open" );
        assert( m_writers.top() == Writer::Object );
        endObject();
        m_stream << '\n' << std::flush;
        assert( m_writers.empty() );
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
        assert( isInside( Writer::Object ) );
        m_objectWriters.pop();
        m_writers.pop();
    }
    void JsonReporter::endArray() {
        assert( isInside( Writer::Array ) );
        m_arrayWriters.pop();
        m_writers.pop();
    }

    bool JsonReporter::isInside( Writer writer ) {
        return !m_writers.empty() && m_writers.top() == writer;
    }

    void JsonReporter::startListing() {
        if ( !m_startedListing ) { startObject( "listings" ); }
        m_startedListing = true;
    }
    void JsonReporter::endListing() {
        if ( m_startedListing ) { endObject(); }
        m_startedListing = false;
    }

    std::string JsonReporter::getDescription() {
        return "WIP! Reports test results as a JSON document. WIP!";
    }

    void JsonReporter::testRunStarting( TestRunInfo const& testInfo ) {
        StreamingReporterBase::testRunStarting( testInfo );
        endListing();

        assert( isInside( Writer::Object ) );
        startObject( "test-run" );
        startArray( "test-cases" );
    }

     static void writeCounts( JsonObjectWriter&& writer, Counts const& counts ) {
        writer.write( "passed" ).write( counts.passed );
        writer.write( "failed" ).write( counts.failed );
        writer.write( "fail-but-ok" ).write( counts.failedButOk );
        writer.write( "skipped" ).write( counts.skipped );
    }

    void JsonReporter::testRunEnded(TestRunStats const& runStats) {
        assert( isInside( Writer::Array ) );
        // End "test-cases"
        endArray();

        {
            auto totals = m_objectWriters.top().write( "totals" ).writeObject();
            writeCounts( totals.write( "assertions" ).writeObject(),
                         runStats.totals.assertions );
            writeCounts( totals.write( "test-cases" ).writeObject(),
                         runStats.totals.testCases );
        }

        // End the "test-run" object
        endObject();
    }

    void JsonReporter::testCaseStarting( TestCaseInfo const& tcInfo ) {
        StreamingReporterBase::testCaseStarting( tcInfo );

        assert( isInside( Writer::Array ) &&
                "We should be in the 'test-cases' array" );
        startObject();
        // "test-info" prelude
        {
            auto testInfo =
                m_objectWriters.top().write( "test-info" ).writeObject();
            // TODO: handle testName vs className!!
            testInfo.write( "name" ).write( tcInfo.name );
            writeSourceInfo(testInfo, tcInfo.lineInfo);
            writeTags( testInfo.write( "tags" ).writeArray(), tcInfo.tags );
            writeProperties( testInfo.write( "properties" ).writeArray(),
                             tcInfo );
        }


        // Start the array for individual test runs (testCasePartial pairs)
        startArray( "runs" );
    }

    void JsonReporter::testCaseEnded( TestCaseStats const& tcStats ) {
        StreamingReporterBase::testCaseEnded( tcStats );

        // We need to close the 'runs' array before finishing the test case
        assert( isInside( Writer::Array ) );
        endArray();

        {
            auto totals = m_objectWriters.top().write( "totals" ).writeObject();
            writeCounts( totals.write( "assertions" ).writeObject(),
                         tcStats.totals.assertions );
            // We do not write the test case totals, because there will always be just one test case here.
            // TODO: overall "result" -> success, skip, fail here? Or in partial result?
        }
        // We do not write out stderr/stdout, because we instead wrote those out in partial runs

        // TODO: aborting?

        // And we also close this test case's object
        assert( isInside( Writer::Object ) );
        endObject();
    }

    void JsonReporter::testCasePartialStarting( TestCaseInfo const& /*tcInfo*/,
                                                uint64_t index ) {
        startObject();
        m_objectWriters.top().write( "run-idx" ).write( index );
        startArray( "path" );
        //startObject( "path" );
        // TODO: we want to delay most of the printing to the 'root' section
        // TODO: childSection key name?
    }

    void JsonReporter::testCasePartialEnded( TestCaseStats const& tcStats,
                                             uint64_t /*index*/ ) {
        // Fixme: the top level section handles this.
        //// path object
        endArray();
        if ( !tcStats.stdOut.empty() ) {
            m_objectWriters.top()
                .write( "captured-stdout" )
                .write( tcStats.stdOut );
        }
        if ( !tcStats.stdErr.empty() ) {
            m_objectWriters.top()
                .write( "captured-stderr" )
                .write( tcStats.stdErr );
        }
        {
            auto totals = m_objectWriters.top().write( "totals" ).writeObject();
            writeCounts( totals.write( "assertions" ).writeObject(),
                         tcStats.totals.assertions );
            // We do not write the test case totals, because there will
            // always be just one test case here.
            // TODO: overall "result" -> success, skip, fail here? Or in
            // partial result?
        }
        // TODO: aborting?
        // run object
        endObject();
    }

    void JsonReporter::sectionStarting( SectionInfo const& sectionInfo ) {
        assert( isInside( Writer::Array ) &&
                "Section should always start inside an object" );
        // We want to nest top level sections, even though it shares name
        // and source loc with the TEST_CASE
        auto& sectionObject = startObject();
        sectionObject.write( "kind" ).write( "section" );
        sectionObject.write( "name" ).write( sectionInfo.name );
        writeSourceInfo( m_objectWriters.top(), sectionInfo.lineInfo );


        // TBD: Do we want to create this event lazily? It would become
        //      rather complex, but we could do it, and it would look
        //      better for empty sections. OTOH, empty sections should
        //      be rare.
        startArray( "path" );
    }
    void JsonReporter::sectionEnded( SectionStats const& /*sectionStats */) {
        // End the subpath array
        endArray();
        // TODO: metadata
        // TODO: what info do we have here?

        // End the section object
        endObject();
    }

    void JsonReporter::assertionStarting( AssertionInfo const& /*assertionInfo*/ ) {}
    void JsonReporter::assertionEnded( AssertionStats const& assertionStats ) {
        // TODO: There is lot of different things to handle here, but
        //       we can fill it in later, after we show that the basic
        //       outline and streaming reporter impl works well enough.
        //if ( !m_config->includeSuccessfulResults()
        //    && assertionStats.assertionResult.isOk() ) {
        //    return;
        //}
        assert( isInside( Writer::Array ) );
        auto assertionObject = m_arrayWriters.top().writeObject();

        assertionObject.write( "kind" ).write( "assertion" );
        writeSourceInfo( assertionObject,
                         assertionStats.assertionResult.getSourceInfo() );
        assertionObject.write( "status" )
            .write( assertionStats.assertionResult.isOk() );
        // TODO: handling of result.
        // TODO: messages
        // TODO: totals?
    }


    void JsonReporter::benchmarkPreparing( StringRef name ) { (void)name; }
    void JsonReporter::benchmarkStarting( BenchmarkInfo const& ) {}
    void JsonReporter::benchmarkEnded( BenchmarkStats<> const& ) {}
    void JsonReporter::benchmarkFailed( StringRef error ) { (void)error; }

    void JsonReporter::listReporters(
        std::vector<ReporterDescription> const& descriptions ) {
        startListing();

        auto writer = m_objectWriters.top().write( "reporters" ).writeArray();
        for ( auto const& desc : descriptions ) {
            auto desc_writer = writer.writeObject();
            desc_writer.write( "name" ).write( desc.name );
            desc_writer.write( "description" ).write( desc.description );
        }
    }
    void JsonReporter::listListeners(
        std::vector<ListenerDescription> const& descriptions ) {
        startListing();

        auto writer = m_objectWriters.top().write( "listeners" ).writeArray();

        for ( auto const& desc : descriptions ) {
            auto desc_writer = writer.writeObject();
            desc_writer.write( "name" ).write( desc.name );
            desc_writer.write( "description" ).write( desc.description );
        }
    }
    void JsonReporter::listTests( std::vector<TestCaseHandle> const& tests ) {
        startListing();

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
        startListing();

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
