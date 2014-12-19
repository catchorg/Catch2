/*
 *  Created by Phil Nash on 19th December 2014
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_TEAMCITY_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_TEAMCITY_HPP_INCLUDED

#include "catch_reporter_bases.hpp"

#include "../internal/catch_reporter_registrars.hpp"

#include <cstring>

namespace Catch {
    
    struct TeamCityReporter : StreamingReporterBase {
        TeamCityReporter( ReporterConfig const& _config )
        :   StreamingReporterBase( _config )
        {}
        
        static std::string escape( std::string const& str ) {
            std::string escaped = str;
            replaceInPlace( escaped, "|", "||" );
            replaceInPlace( escaped, "\'", "|\'" );
            replaceInPlace( escaped, "\n", "|n" );
            replaceInPlace( escaped, "\r", "|r" );
            replaceInPlace( escaped, "[", "|[" );
            replaceInPlace( escaped, "]", "|]" );
            return escaped;
        }
        virtual ~TeamCityReporter();

        static std::string getDescription() {
            return "Reports test results as TeamCity service messages";
        }
        virtual ReporterPreferences getPreferences() const {
            ReporterPreferences prefs;
            prefs.shouldRedirectStdOut = true;
            return prefs;
        }
        
        // !TBD: ignored tests
        
        virtual void noMatchingTestCases( std::string const& /* spec */ ) {}
        
        virtual void testGroupStarting( GroupInfo const& groupInfo ) {
            StreamingReporterBase::testGroupStarting( groupInfo );
            stream << "##teamcity[testSuiteStarted name='"
                << escape( groupInfo.name ) << "']\n";
        }
        virtual void testGroupEnded( TestGroupStats const& testGroupStats ) {
            StreamingReporterBase::testGroupEnded( testGroupStats );
            stream << "##teamcity[testSuiteFinished name='"
                << escape( testGroupStats.groupInfo.name ) << "']\n";
        }

        
        virtual void assertionStarting( AssertionInfo const& ) {
        }
        
        virtual bool assertionEnded( AssertionStats const& assertionStats ) {
            AssertionResult const& result = assertionStats.assertionResult;
            if( !result.isOk() ) {
                
                std::string message;
                std::string details;
                switch( result.getResultType() ) {
                    case ResultWas::ExpressionFailed:
                        message = "expression failed";
                        break;
                    case ResultWas::ThrewException:
                        message = "unexpected exception";
                        break;
                    case ResultWas::FatalErrorCondition:
                        message = "fatal error condition";
                        break;
                    case ResultWas::DidntThrowException:
                        message = "no exception was thrown where one was expected";
                        break;
                    case ResultWas::ExplicitFailure:
                        message = "explicit failure";
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
                    message += " with message:";
                if( assertionStats.infoMessages.size() > 1 )
                    message += " with messages:";
                for( std::vector<MessageInfo>::const_iterator
                        it = assertionStats.infoMessages.begin(),
                        itEnd = assertionStats.infoMessages.end();
                    it != itEnd;
                    ++it )
                    message += "\n" + it->message;
                
                
                if( result.hasExpression() ) {
                    details =
                        "  " + result.getExpressionInMacro() + "\n"
                        "with expansion:\n" +
                        "  " + result.getExpandedExpression() + "\n";
                }
                
                // !TBD: file/ line
                
                stream << "##teamcity[testFailed"
                    << " name='" << escape( currentTestCaseInfo->name )<< "'"
                    << " message='" << escape( message ) << "'"
                    << " details='" << escape( details ) << "'"
                    << "]\n";
            }
            return true;
        }
        
//        virtual void sectionStarting( SectionInfo const& _sectionInfo ) {
//            // !TBD
//        }
//        virtual void sectionEnded( SectionStats const& _sectionStats ) {
//            // !TBD
//        }
        
        virtual void testCaseStarting( TestCaseInfo const& testInfo ) {
            StreamingReporterBase::testCaseStarting( testInfo );
            stream << "##teamcity[testStarted name='"
                << escape( testInfo.name ) << "']\n";
        }
        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) {
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
//        virtual void testRunEnded( TestRunStats const& _testRunStats ) {
//            // !TBD
//        }
        
    private:
        
    };
    
#ifdef CATCH_IMPL
    TeamCityReporter::~TeamCityReporter() {}
#endif
    
    INTERNAL_CATCH_REGISTER_REPORTER( "teamcity", TeamCityReporter )
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_TEAMCITY_HPP_INCLUDED
