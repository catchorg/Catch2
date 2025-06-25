
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/catch_test_case_info.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_string_manip.hpp>
#include <catch2/internal/catch_textflow.hpp>
#include <catch2/reporters/catch_reporter_helpers.hpp>
#include <catch2/reporters/catch_reporter_teamcity.hpp>

#include <algorithm>
#include <cassert>
#include <ostream>

namespace Catch {

    namespace {
        std::string escape( StringRef str ) {
            std::string escaped = static_cast<std::string>( str );
            replaceInPlace( escaped, " ", "_" );
            replaceInPlace( escaped, "|", "||" );
            replaceInPlace( escaped, "'", "|'" );
            replaceInPlace( escaped, "\n", "|n" );
            replaceInPlace( escaped, "\r", "|r" );
            replaceInPlace( escaped, "[", "|[" );
            replaceInPlace( escaped, "]", "|]" );
            return escaped;
        }
    } // end anonymous namespace

    TeamCityReporter::TeamCityReporter( ReporterConfig&& _config ):
        StreamingReporterBase( CATCH_MOVE( _config ) ) {
        m_preferences.shouldRedirectStdOut = true;

        parseCustomOptions();
    }

    TeamCityReporter::~TeamCityReporter() = default;

    void TeamCityReporter::testRunStarting( TestRunInfo const& runInfo ) {
        StreamingReporterBase::testRunStarting( runInfo );
        m_stream << "##teamcity[testSuiteStarted name='"
                 << escape( runInfo.name ) << "']\n";
    }

    void TeamCityReporter::testRunEnded( TestRunStats const& runStats ) {
        StreamingReporterBase::testRunEnded( runStats );

        m_stream << "##teamcity[testSuiteFinished name='"
                 << escape( runStats.runInfo.name ) << "']\n";
    }

    void TeamCityReporter::assertionEnded( AssertionStats const& assertionStats ) {
        AssertionResult const& result = assertionStats.assertionResult;
        if ( !result.isOk() ||
             result.getResultType() == ResultWas::ExplicitSkip ) {

            if ( m_printSections ) {
                m_stream << createTestCaseHeader( printSectionName() );
                m_headerPrintedForThisSection = true;
            }

            ReusableStringStream msg;
            msg << result.getSourceInfo() << '\n';

            switch (result.getResultType()) {
            case ResultWas::ExpressionFailed:
                msg << "expression failed";
                break;
            case ResultWas::ThrewException:
                msg << "unexpected exception";
                break;
            case ResultWas::FatalErrorCondition:
                msg << "fatal error condition";
                break;
            case ResultWas::DidntThrowException:
                msg << "no exception was thrown where one was expected";
                break;
            case ResultWas::ExplicitFailure:
                msg << "explicit failure";
                break;
            case ResultWas::ExplicitSkip:
                msg << "explicit skip";
                break;

                // We shouldn't get here because of the isOk() test
            case ResultWas::Ok:
            case ResultWas::Info:
            case ResultWas::Warning:
                CATCH_ERROR("Internal error in TeamCity reporter");
                // These cases are here to prevent compiler warnings
            case ResultWas::Unknown:
            case ResultWas::FailureBit:
            case ResultWas::Exception:
                CATCH_ERROR("Not implemented");
            }
            if (assertionStats.infoMessages.size() == 1)
                msg << " with message:";
            if (assertionStats.infoMessages.size() > 1)
                msg << " with messages:";
            for (auto const& messageInfo : assertionStats.infoMessages)
                msg << "\n  \"" << messageInfo.message << '"';

            if (result.hasExpression()) {
                msg << "\n  " << result.getExpressionInMacro()
                    << "\n"
                       "with expansion:\n"
                       "  "
                    << result.getExpandedExpression() << '\n';
            }

            if ( result.getResultType() == ResultWas::ExplicitSkip ) {
                m_stream << "##teamcity[testIgnored";
            } else if ( currentTestCaseInfo->okToFail() ) {
                msg << "- failure ignore as test marked as 'ok to fail'\n";
                m_stream << "##teamcity[testIgnored";
            } else {
                m_stream << "##teamcity[testFailed";
            }

            if ( m_printSections ) {
                m_stream << " name='" << escape( printSectionName() ) << '\'';
            } else {
                m_stream << " name='" << escape( currentTestCaseInfo->name )
                         << '\'';
            }
            m_stream << " message='" << escape( msg.str() ) << "']\n";
        }
    }

    void TeamCityReporter::testCaseStarting( TestCaseInfo const& testInfo ) {
        StreamingReporterBase::testCaseStarting( testInfo );

        if ( !m_printSections ) {
            m_testCaseTimer.start();
            m_stream << createTestCaseHeader( testInfo.name );
        }
    }

    void TeamCityReporter::testCaseEnded( TestCaseStats const& testCaseStats ) {
        StreamingReporterBase::testCaseEnded( testCaseStats );

        if ( !m_printSections ) {
            m_stream << createTestCaseFooter(
                testCaseStats.testInfo->name,
                m_testCaseTimer.getElapsedSeconds() );
        }
    }

    std::string TeamCityReporter::printSectionName() {
        std::string output;

        for ( const auto& entry : m_sectionStack ) {
            output += entry.name + m_sectionSeparator;
        }

        const auto endPos = output.find_last_not_of( m_sectionSeparator );
        if ( endPos != std::string::npos ) { output.resize( endPos + 1 ); }

        return output;
    }

    void TeamCityReporter::sectionStarting( SectionInfo const& sectionInfo ) {
        StreamingReporterBase::sectionStarting( sectionInfo );

        if ( !m_printSections ) { return; }

        m_headerPrintedForThisSection = false;
    }

    void TeamCityReporter::sectionEnded( SectionStats const& sectionStats ) {
        if ( !m_printSections ) { return; }

        if ( !m_headerPrintedForThisSection ) {
            m_headerPrintedForThisSection = true;

            m_stream << createTestCaseHeader( printSectionName() );
            m_stream << createTestCaseFooter( printSectionName(),
                                              sectionStats.durationInSeconds );
        }

        StreamingReporterBase::sectionEnded( sectionStats );
    }

    std::string TeamCityReporter::createTestCaseHeader( std::string name ) {
        std::string result{ "##teamcity[testStarted name='" };
        result += escape( name );
        result += "']\n";
        return result;
    }

    std::string TeamCityReporter::createTestCaseFooter( std::string name,
                                                        double duration ) {
        std::string result{ "##teamcity[testFinished name='" };
        result += escape( name );

        if ( m_config->showDurations() == ShowDurations::Always ) {
            result +=
                "' duration='" +
                std::to_string( static_cast<uint32_t>( duration * 1000 ) );
        }

        result += "']\n";
        return result;
    }

    void TeamCityReporter::parseCustomOptions() {
        auto result = m_customOptions.find( "Xsections" );
        if ( result != m_customOptions.end() ) {
            m_printSections = result->second == "true";
        }

        result = m_customOptions.find( "Xseparator" );
        if ( result != m_customOptions.end() ) {
            m_sectionSeparator = result->second;
        }
    }

} // end namespace Catch
