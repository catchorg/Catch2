/*
 *  Created by Martin on 19/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_interfaces_reporter.h"
#include "../reporters/catch_reporter_multi.h"

namespace Catch {

    ReporterConfig::ReporterConfig( IConfigPtr const& _fullConfig )
    :   m_stream( &_fullConfig->stream() ), m_fullConfig( _fullConfig ) {}

    ReporterConfig::ReporterConfig( IConfigPtr const& _fullConfig, std::ostream& _stream )
    :   m_stream( &_stream ), m_fullConfig( _fullConfig ) {}

    std::ostream& ReporterConfig::stream() const { return *m_stream; }
    IConfigPtr ReporterConfig::fullConfig() const { return m_fullConfig; }


    TestRunInfo::TestRunInfo( std::string const& _name ) : name( _name ) {}

    GroupInfo::GroupInfo(  std::string const& _name,
                           std::size_t _groupIndex,
                           std::size_t _groupsCount )
    :   name( _name ),
        groupIndex( _groupIndex ),
        groupsCounts( _groupsCount )
    {}

     AssertionStats::AssertionStats( AssertionResult const& _assertionResult,
                                     std::vector<MessageInfo> const& _infoMessages,
                                     Totals const& _totals )
    :   assertionResult( _assertionResult ),
        infoMessages( _infoMessages ),
        totals( _totals )
    {
        assertionResult.m_resultData.lazyExpression.m_transientExpression = _assertionResult.m_resultData.lazyExpression.m_transientExpression;

        if( assertionResult.hasMessage() ) {
            // Copy message into messages list.
            // !TBD This should have been done earlier, somewhere
            MessageBuilder builder( assertionResult.getTestMacroName(), assertionResult.getSourceInfo(), assertionResult.getResultType() );
            builder << assertionResult.getMessage();
            builder.m_info.message = builder.m_stream.str();

            infoMessages.push_back( builder.m_info );
        }
    }

     AssertionStats::~AssertionStats() = default;

    SectionStats::SectionStats(  SectionInfo const& _sectionInfo,
                                 Counts const& _assertions,
                                 double _durationInSeconds,
                                 bool _missingAssertions )
    :   sectionInfo( _sectionInfo ),
        assertions( _assertions ),
        durationInSeconds( _durationInSeconds ),
        missingAssertions( _missingAssertions )
    {}

    SectionStats::~SectionStats() = default;


    TestCaseStats::TestCaseStats(  TestCaseInfo const& _testInfo,
                                   Totals const& _totals,
                                   std::string const& _stdOut,
                                   std::string const& _stdErr,
                                   bool _aborting )
    : testInfo( _testInfo ),
        totals( _totals ),
        stdOut( _stdOut ),
        stdErr( _stdErr ),
        aborting( _aborting )
    {}

    TestCaseStats::~TestCaseStats() = default;


    TestGroupStats::TestGroupStats( GroupInfo const& _groupInfo,
                                    Totals const& _totals,
                                    bool _aborting )
    :   groupInfo( _groupInfo ),
        totals( _totals ),
        aborting( _aborting )
    {}

    TestGroupStats::TestGroupStats( GroupInfo const& _groupInfo )
    :   groupInfo( _groupInfo ),
        aborting( false )
    {}

    TestGroupStats::~TestGroupStats() = default;

    TestRunStats::TestRunStats(   TestRunInfo const& _runInfo,
                    Totals const& _totals,
                    bool _aborting )
    :   runInfo( _runInfo ),
        totals( _totals ),
        aborting( _aborting )
    {}

    TestRunStats::~TestRunStats() = default;

    void IStreamingReporter::fatalErrorEncountered( StringRef ) {}
    bool IStreamingReporter::isMulti() const { return false; }

    IReporterFactory::~IReporterFactory() = default;
    IReporterRegistry::~IReporterRegistry() = default;

    void addReporter( IStreamingReporterPtr& existingReporter, IStreamingReporterPtr&& additionalReporter ) {

        if( !existingReporter ) {
            existingReporter = std::move( additionalReporter );
            return;
        }

        MultipleReporters* multi = nullptr;

        if( existingReporter->isMulti() ) {
            multi = static_cast<MultipleReporters*>( existingReporter.get() );
        }
        else {
            auto newMulti = std::unique_ptr<MultipleReporters>( new MultipleReporters );
            newMulti->add( std::move( existingReporter ) );
            multi = newMulti.get();
            existingReporter = std::move( newMulti );
        }
        multi->add( std::move( additionalReporter ) );
    }

} // end namespace Catch
