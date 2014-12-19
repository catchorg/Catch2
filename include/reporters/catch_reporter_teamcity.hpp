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
        
        static bool replace( std::string& str, std::string const& replaceThis, std::string const& withThis ) {
            std::size_t i = str.find( replaceThis );
            if( i != std::string::npos ) {
                str = str.substr( 0, i ) + withThis + str.substr( i+replaceThis.size() );
                return true;
            }
            return false;
        }
        static std::string escape( std::string const& str ) {
            std::string escaped = str;
            while(  replace( escaped, "\'", "|\'" ) ||
                    replace( escaped, "\n", "|n" ) ||
                    replace( escaped, "\r", "|r" ) ||
                    replace( escaped, "|", "||" ) ||
                    replace( escaped, "[", "|[" ) ||
                    replace( escaped, "]", "|]" ) );
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
            if( !assertionStats.assertionResult.isOk() ) {
                stream << "##teamcity[testFailed"
                    << " name='" << escape( currentTestCaseInfo->name )<< "'"
                    << " message='message here'" // !TBD
                    << " details='details?'" // !TBD
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
