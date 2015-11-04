/*
 *  Created by Phil Nash on 19th December 2014
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_TEAMCITY_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_TEAMCITY_HPP_INCLUDED

// Don't #include any Catch headers here - we can assume they are already
// included before this header.
// This is not good practice in general but is necessary in this case so this
// file can be distributed as a single header that works with the main
// Catch single header.

#include <cstring>

#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wc++98-compat"
#   pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#endif

namespace Catch {

    struct TeamCityReporter : StreamingReporterBase {
        TeamCityReporter( ReporterConfig const& _config )
        :   StreamingReporterBase( _config ),
            m_headerPrintedForThisSection( false )
        {
            m_reporterPrefs.shouldRedirectStdOut = true;
        }

        static std::string escape( std::string const& str ) {
            std::string escaped = str;
            replaceInPlace( escaped, "|", "||" );
            replaceInPlace( escaped, "'", "|'" );
            replaceInPlace( escaped, "\n", "|n" );
            replaceInPlace( escaped, "\r", "|r" );
            replaceInPlace( escaped, "[", "|[" );
            replaceInPlace( escaped, "]", "|]" );
            return escaped;
        }
        virtual ~TeamCityReporter() CATCH_OVERRIDE;

        static std::string getDescription() {
            return "Reports test results as TeamCity service messages";
        }

        virtual void skipTest( TestCaseInfo const& testInfo ) CATCH_OVERRIDE {
            stream  << "##teamcity[testIgnored name='"
                    << escape( testInfo.name ) << "'";
            if( testInfo.isHidden() )
                stream << " message='hidden test'";
            else
                stream << " message='test skipped because it didn|'t match the test spec'";
            stream << "]\n";
        }

        virtual void noMatchingTestCases( std::string const& /* spec */ ) CATCH_OVERRIDE {}

        virtual void testGroupStarting( GroupInfo const& groupInfo ) CATCH_OVERRIDE {
            StreamingReporterBase::testGroupStarting( groupInfo );
            stream << "##teamcity[testSuiteStarted name='"
                << escape( groupInfo.name ) << "']\n";
        }
        virtual void testGroupEnded( TestGroupStats const& testGroupStats ) CATCH_OVERRIDE {
            StreamingReporterBase::testGroupEnded( testGroupStats );
            stream << "##teamcity[testSuiteFinished name='"
                << escape( testGroupStats.groupInfo.name ) << "']\n";
        }


        virtual void assertionStarting( AssertionInfo const& ) CATCH_OVERRIDE {
        }

        virtual bool assertionEnded( AssertionStats const& assertionStats ) CATCH_OVERRIDE {
            AssertionResult const& result = assertionStats.assertionResult;
            if( !result.isOk() ) {

                std::ostringstream msg;
                if( !m_headerPrintedForThisSection )
                    printSectionHeader( msg );
                m_headerPrintedForThisSection = true;

                msg << result.getSourceInfo() << "\n";

                switch( result.getResultType() ) {
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

                    // We shouldn't get here because of the isOk() test
                    case ResultWas::Ok:
                    case ResultWas::Info:
                    case ResultWas::Warning:

                    // These cases are here to prevent compiler warnings
                    case ResultWas::Unknown:
                    case ResultWas::FailureBit:
                    case ResultWas::Exception:
                        CATCH_NOT_IMPLEMENTED;
                }
                if( assertionStats.infoMessages.size() == 1 )
                    msg << " with message:";
                if( assertionStats.infoMessages.size() > 1 )
                    msg << " with messages:";
                for( std::vector<MessageInfo>::const_iterator
                        it = assertionStats.infoMessages.begin(),
                        itEnd = assertionStats.infoMessages.end();
                    it != itEnd;
                    ++it )
                    msg << "\n  \"" << it->message << "\"";


                if( result.hasExpression() ) {
                    msg <<
                        "\n  " << result.getExpressionInMacro() << "\n"
                        "with expansion:\n" <<
                        "  " << result.getExpandedExpression() << "\n";
                }

                stream << "##teamcity[testFailed"
                    << " name='" << escape( currentTestCaseInfo->name )<< "'"
                    << " message='" << escape( msg.str() ) << "'"
                    << "]\n";
            }
            return true;
        }

        virtual void sectionStarting( SectionInfo const& sectionInfo ) CATCH_OVERRIDE {
            m_headerPrintedForThisSection = false;
            StreamingReporterBase::sectionStarting( sectionInfo );
        }

        virtual void testCaseStarting( TestCaseInfo const& testInfo ) CATCH_OVERRIDE {
            StreamingReporterBase::testCaseStarting( testInfo );
            stream << "##teamcity[testStarted name='"
                << escape( testInfo.name ) << "']\n";
        }

        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) CATCH_OVERRIDE {
            StreamingReporterBase::testCaseEnded( testCaseStats );
            if( !testCaseStats.stdOut.empty() )
                stream << "##teamcity[testStdOut name='"
                    << escape( testCaseStats.testInfo.name )
                    << "' out='" << escape( testCaseStats.stdOut ) << "']\n";
            if( !testCaseStats.stdErr.empty() )
                stream << "##teamcity[testStdErr name='"
                    << escape( testCaseStats.testInfo.name )
                    << "' out='" << escape( testCaseStats.stdErr ) << "']\n";
            stream << "##teamcity[testFinished name='"
                << escape( testCaseStats.testInfo.name ) << "']\n";
        }

    private:
        void printSectionHeader( std::ostream& os ) {
            assert( !m_sectionStack.empty() );

            if( m_sectionStack.size() > 1 ) {
                os << getLineOfChars<'-'>() << "\n";

                std::vector<SectionInfo>::const_iterator
                it = m_sectionStack.begin()+1, // Skip first section (test case)
                itEnd = m_sectionStack.end();
                for( ; it != itEnd; ++it )
                    printHeaderString( os, it->name );
                os << getLineOfChars<'-'>() << "\n";
            }

            SourceLineInfo lineInfo = m_sectionStack.front().lineInfo;

            if( !lineInfo.empty() )
                os << lineInfo << "\n";
            os << getLineOfChars<'.'>() << "\n\n";
        }

        // if string has a : in first line will set indent to follow it on
        // subsequent lines
        void printHeaderString( std::ostream& os, std::string const& _string, std::size_t indent = 0 ) {
            std::size_t i = _string.find( ": " );
            if( i != std::string::npos )
                i+=2;
            else
                i = 0;
            os << Text( _string, TextAttributes()
                           .setIndent( indent+i)
                           .setInitialIndent( indent ) ) << "\n";
        }
    private:
        bool m_headerPrintedForThisSection;

    };

#ifdef CATCH_IMPL
    TeamCityReporter::~TeamCityReporter() {}
#endif

    INTERNAL_CATCH_REGISTER_REPORTER( "teamcity", TeamCityReporter )

} // end namespace Catch

#ifdef __clang__
#   pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_REPORTER_TEAMCITY_HPP_INCLUDED
