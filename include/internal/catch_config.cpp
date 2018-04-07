/*
 *  Created by Martin on 19/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_config.hpp"
#include "catch_enforce.h"
#include "catch_stringref.h"

namespace Catch {

    Config::Config( ConfigData const& data )
    :   m_data( data ),
        m_stream( openStream() )
    {
        TestSpecParser parser(ITagAliasRegistry::get());
        if (data.testsOrTags.empty()) {
            parser.parse("~[.]"); // All not hidden tests
        }
        else {
            m_hasTestFilters = true;
            for( auto const& testOrTags : data.testsOrTags )
                parser.parse( testOrTags );
        }
        m_testSpec = parser.testSpec();
    }

    std::string const& Config::getFilename() const {
        return m_data.outputFilename ;
    }

    bool Config::listTests() const          { return m_data.listTests; }
    bool Config::listTestNamesOnly() const  { return m_data.listTestNamesOnly; }
    bool Config::listTags() const           { return m_data.listTags; }
    bool Config::listReporters() const      { return m_data.listReporters; }

    std::string Config::getProcessName() const { return m_data.processName; }
    std::string const& Config::getReporterName() const { return m_data.reporterName; }

    std::vector<std::string> const& Config::getTestsOrTags() const { return m_data.testsOrTags; }
    std::vector<std::string> const& Config::getSectionsToRun() const { return m_data.sectionsToRun; }

    TestSpec const& Config::testSpec() const { return m_testSpec; }
    bool Config::hasTestFilters() const { return m_hasTestFilters; }

    bool Config::showHelp() const { return m_data.showHelp; }

    // IConfig interface
    bool Config::allowThrows() const                   { return !m_data.noThrow; }
    std::ostream& Config::stream() const               { return m_stream->stream(); }
    std::string Config::name() const                   { return m_data.name.empty() ? m_data.processName : m_data.name; }
    bool Config::includeSuccessfulResults() const      { return m_data.showSuccessfulTests; }
    bool Config::warnAboutMissingAssertions() const    { return !!(m_data.warnings & WarnAbout::NoAssertions); }
    bool Config::warnAboutNoTests() const              { return !!(m_data.warnings & WarnAbout::NoTests); }
    ShowDurations::OrNot Config::showDurations() const { return m_data.showDurations; }
    RunTests::InWhatOrder Config::runOrder() const     { return m_data.runOrder; }
    unsigned int Config::rngSeed() const               { return m_data.rngSeed; }
    int Config::benchmarkResolutionMultiple() const    { return m_data.benchmarkResolutionMultiple; }
    UseColour::YesOrNo Config::useColour() const       { return m_data.useColour; }
    bool Config::shouldDebugBreak() const              { return m_data.shouldDebugBreak; }
    int Config::abortAfter() const                     { return m_data.abortAfter; }
    bool Config::showInvisibles() const                { return m_data.showInvisibles; }
    Verbosity Config::verbosity() const                { return m_data.verbosity; }

    IStream const* Config::openStream() {
        return Catch::makeStream(m_data.outputFilename);
    }

} // end namespace Catch
