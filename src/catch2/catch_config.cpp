
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/catch_config.hpp>
#include <catch2/catch_user_config.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_stream.hpp>
#include <catch2/internal/catch_stringref.hpp>
#include <catch2/internal/catch_string_manip.hpp>
#include <catch2/internal/catch_test_spec_parser.hpp>
#include <catch2/interfaces/catch_interfaces_tag_alias_registry.hpp>

#include <ostream>

namespace Catch {
    namespace Detail {
        namespace {
            class RDBufStream : public IStream {
                mutable std::ostream m_os;

            public:
                //! The streambuf `sb` must outlive the constructed object.
                RDBufStream( std::streambuf* sb ): m_os( sb ) {}
                ~RDBufStream() override = default;

            public: // IStream
                std::ostream& stream() const override { return m_os; }
            };
        } // unnamed namespace
    }     // namespace Detail

    Config::Config( ConfigData const& data ):
        m_data( data ) {
        // We need to trim filter specs to avoid trouble with superfluous
        // whitespace (esp. important for bdd macros, as those are manually
        // aligned with whitespace).

        for (auto& elem : m_data.testsOrTags) {
            elem = trim(elem);
        }
        for (auto& elem : m_data.sectionsToRun) {
            elem = trim(elem);
        }


        TestSpecParser parser(ITagAliasRegistry::get());
        if (!m_data.testsOrTags.empty()) {
            m_hasTestFilters = true;
            for (auto const& testOrTags : m_data.testsOrTags) {
                parser.parse(testOrTags);
            }
        }
        m_testSpec = parser.testSpec();


        // Insert the default reporter if user hasn't asked for a specfic one
        if ( m_data.reporterSpecifications.empty() ) {
            m_data.reporterSpecifications.push_back( {
#if defined( CATCH_CONFIG_DEFAULT_REPORTER )
                CATCH_CONFIG_DEFAULT_REPORTER,
#else
                "console",
#endif
                {}, {}, {}
            } );
        }

#if defined( CATCH_CONFIG_BAZEL_SUPPORT )
        // Register a JUnit reporter for Bazel. Bazel sets an environment
        // variable with the path to XML output. If this file is written to
        // during test, Bazel will not generate a default XML output.
        // This allows the XML output file to contain higher level of detail
        // than what is possible otherwise.
#    if defined( _MSC_VER )
        // On Windows getenv throws a warning as there is no input validation,
        // since the key is hardcoded, this should not be an issue.
#        pragma warning( push )
#        pragma warning( disable : 4996 )
#    endif
        const auto bazelOutputFilePtr = std::getenv( "XML_OUTPUT_FILE" );
#    if defined( _MSC_VER )
#        pragma warning( pop )
#    endif
        if ( bazelOutputFilePtr != nullptr ) {
            m_data.reporterSpecifications.push_back(
                { "junit", std::string( bazelOutputFilePtr ), {}, {} } );
        }
#endif

        bool defaultOutputUsed = false;
        m_reporterStreams.reserve( m_data.reporterSpecifications.size() );
        for ( auto const& reporterSpec : m_data.reporterSpecifications ) {
            if ( reporterSpec.outputFile().none() ) {
                CATCH_ENFORCE( !defaultOutputUsed,
                               "Internal error: cannot use default output for "
                               "multiple reporters" );
                defaultOutputUsed = true;

                m_reporterStreams.push_back(
                    makeStream( data.defaultOutputFilename ) );
            } else {
                m_reporterStreams.push_back(
                    makeStream( *reporterSpec.outputFile() ) );
            }
        }
    }

    Config::~Config() = default;


    bool Config::listTests() const          { return m_data.listTests; }
    bool Config::listTags() const           { return m_data.listTags; }
    bool Config::listReporters() const      { return m_data.listReporters; }

    std::vector<std::string> const& Config::getTestsOrTags() const { return m_data.testsOrTags; }
    std::vector<std::string> const& Config::getSectionsToRun() const { return m_data.sectionsToRun; }

    std::vector<ReporterSpec> const& Config::getReporterSpecs() const {
        return m_data.reporterSpecifications;
    }

    IStream const* Config::getReporterOutputStream(std::size_t reporterIdx) const {
        return m_reporterStreams.at(reporterIdx).get();
    }

    TestSpec const& Config::testSpec() const { return m_testSpec; }
    bool Config::hasTestFilters() const { return m_hasTestFilters; }

    bool Config::showHelp() const { return m_data.showHelp; }

    // IConfig interface
    bool Config::allowThrows() const                   { return !m_data.noThrow; }
    StringRef Config::name() const { return m_data.name.empty() ? m_data.processName : m_data.name; }
    bool Config::includeSuccessfulResults() const      { return m_data.showSuccessfulTests; }
    bool Config::warnAboutMissingAssertions() const {
        return !!( m_data.warnings & WarnAbout::NoAssertions );
    }
    bool Config::warnAboutUnmatchedTestSpecs() const {
        return !!( m_data.warnings & WarnAbout::UnmatchedTestSpec );
    }
    bool Config::zeroTestsCountAsSuccess() const       { return m_data.allowZeroTests; }
    ShowDurations Config::showDurations() const        { return m_data.showDurations; }
    double Config::minDuration() const                 { return m_data.minDuration; }
    TestRunOrder Config::runOrder() const              { return m_data.runOrder; }
    uint32_t Config::rngSeed() const                   { return m_data.rngSeed; }
    unsigned int Config::shardCount() const            { return m_data.shardCount; }
    unsigned int Config::shardIndex() const            { return m_data.shardIndex; }
    ColourMode Config::defaultColourMode() const       { return m_data.defaultColourMode; }
    bool Config::shouldDebugBreak() const              { return m_data.shouldDebugBreak; }
    int Config::abortAfter() const                     { return m_data.abortAfter; }
    bool Config::showInvisibles() const                { return m_data.showInvisibles; }
    Verbosity Config::verbosity() const                { return m_data.verbosity; }

    bool Config::benchmarkNoAnalysis() const                      { return m_data.benchmarkNoAnalysis; }
    unsigned int Config::benchmarkSamples() const                 { return m_data.benchmarkSamples; }
    double Config::benchmarkConfidenceInterval() const            { return m_data.benchmarkConfidenceInterval; }
    unsigned int Config::benchmarkResamples() const               { return m_data.benchmarkResamples; }
    std::chrono::milliseconds Config::benchmarkWarmupTime() const { return std::chrono::milliseconds(m_data.benchmarkWarmupTime); }

} // end namespace Catch
