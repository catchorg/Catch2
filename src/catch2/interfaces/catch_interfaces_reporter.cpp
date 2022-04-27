
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>
#include <catch2/internal/catch_console_colour.hpp>
#include <catch2/internal/catch_console_width.hpp>
#include <catch2/catch_message.hpp>
#include <catch2/internal/catch_list.hpp>
#include <catch2/internal/catch_string_manip.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/reporters/catch_reporter_helpers.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_istream.hpp>

#include <algorithm>
#include <cassert>
#include <iomanip>

namespace Catch {

    ReporterConfig::ReporterConfig(
        IConfig const* _fullConfig,
        Detail::unique_ptr<IStream> _stream,
        ColourMode colourMode,
        std::map<std::string, std::string> customOptions ):
        m_stream( CATCH_MOVE(_stream) ),
        m_fullConfig( _fullConfig ),
        m_colourMode( colourMode ),
        m_customOptions( CATCH_MOVE( customOptions ) ) {}

    Detail::unique_ptr<IStream> ReporterConfig::takeStream() && {
        assert( m_stream );
        return CATCH_MOVE( m_stream );
    }
    IConfig const * ReporterConfig::fullConfig() const { return m_fullConfig; }
    ColourMode ReporterConfig::colourMode() const { return m_colourMode; }

    std::map<std::string, std::string> const&
    ReporterConfig::customOptions() const {
        return m_customOptions;
    }

    ReporterConfig::~ReporterConfig() = default;

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

    SectionStats::SectionStats(  SectionInfo const& _sectionInfo,
                                 Counts const& _assertions,
                                 double _durationInSeconds,
                                 bool _missingAssertions )
    :   sectionInfo( _sectionInfo ),
        assertions( _assertions ),
        durationInSeconds( _durationInSeconds ),
        missingAssertions( _missingAssertions )
    {}


    TestCaseStats::TestCaseStats(  TestCaseInfo const& _testInfo,
                                   Totals const& _totals,
                                   std::string const& _stdOut,
                                   std::string const& _stdErr,
                                   bool _aborting )
    : testInfo( &_testInfo ),
        totals( _totals ),
        stdOut( _stdOut ),
        stdErr( _stdErr ),
        aborting( _aborting )
    {}


    TestRunStats::TestRunStats(   TestRunInfo const& _runInfo,
                    Totals const& _totals,
                    bool _aborting )
    :   runInfo( _runInfo ),
        totals( _totals ),
        aborting( _aborting )
    {}

    IEventListener::~IEventListener() = default;

} // end namespace Catch
