/*
 *  Created by Phil on 5/12/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED

#include "../internal/catch_interfaces_reporter.h"
#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_console_colour.hpp"

namespace Catch {

    struct ConsoleReporter : AccumulatingReporter {
        ConsoleReporter( ReporterConfig const& _config )
        : AccumulatingReporter( _config )
        {}

        virtual ~ConsoleReporter();
        static std::string getDescription() {
            return "Reports test results as plain lines of text";
        }
        virtual ReporterPreferences getPreferences() const {
            ReporterPreferences prefs;
            prefs.shouldRedirectStdOut = false;
            return prefs;

        }
        void lazyPrintRunInfo() {
            stream << "[Started testing: " << testRunInfo->name << "]" << std::endl;
            testRunInfo.reset();
        }
        void lazyPrintGroupInfo() {
            if( !unusedGroupInfo->name.empty() )
                stream << "[Group: '" << unusedGroupInfo->name << "']" << std::endl;
            unusedGroupInfo.reset();
        }
        void lazyPrintTestCaseInfo() {
            stream << "[Test case: '" << unusedTestCaseInfo->name << "']" << std::endl;
            unusedTestCaseInfo.reset();
        }
        std::string makeSectionPath( ThreadedSectionInfo const * section, std::string const& delimiter ) {
            std::string sectionPath = "'" + section->name + "'";
            while( ( section = section->parent.get() ) )
                sectionPath = "'" + section->name + "'" + delimiter + sectionPath;
            return sectionPath;
        }
        void lazyPrintSectionInfo() {
            ThreadedSectionInfo* section = unusedSectionInfo.get();

            std::string sectionPath = makeSectionPath( section, ", " );
            if( sectionPath.size() > 60 )
                sectionPath = makeSectionPath( section, ",\n          " );

            stream << "[Section: " << sectionPath << "]" << std::endl;
            unusedSectionInfo.reset();
        }
        void lazyPrint() {
            if( testRunInfo )
                lazyPrintRunInfo();
            if( unusedGroupInfo )
                lazyPrintGroupInfo();
            if( unusedTestCaseInfo )
                lazyPrintTestCaseInfo();
            if( unusedSectionInfo )
                lazyPrintSectionInfo();
        }

        virtual void assertionStarting( AssertionInfo const& _assertionInfo ) {
        }
        virtual void assertionEnded( Ptr<AssertionStats const> const& _assertionStats ) {
            if( !_assertionStats->assertionResult.isOk() )
                lazyPrint();
        }

        void printAssertionCounts( std::string const& label, Counts const& counts, std::string const& allPrefix = "All " ) {
            if( counts.passed )
                stream << counts.failed << " of " << counts.total() << " " << label << "s failed";
            else
                stream << ( counts.failed > 1 ? allPrefix : "" ) << pluralise( counts.failed, label ) << " failed";
        }

        void printTotals( const Totals& totals, const std::string& allPrefix = "All " ) {
            if( totals.assertions.total() == 0 ) {
                stream << "No tests ran";
            }
            else if( totals.assertions.failed ) {
                TextColour colour( TextColour::ResultError );
                printAssertionCounts( "test case", totals.testCases, allPrefix );
                if( totals.testCases.failed > 0 ) {
                    stream << " (";
                    printAssertionCounts( "assertion", totals.assertions, allPrefix );
                    stream << ")";
                }
            }
            else {
                TextColour colour( TextColour::ResultSuccess );
                stream << allPrefix << "tests passed ("
                    << pluralise( totals.assertions.passed, "assertion" ) << " in "
                    << pluralise( totals.testCases.passed, "test case" ) << ")";
            }
        }

        virtual void sectionEnded( Ptr<SectionStats const> const& _sectionStats ) {
            if( !unusedSectionInfo ) {
                stream << "[Summary for section '" << _sectionStats->sectionInfo.name << "': ";
                printAssertionCounts( "assertion", _sectionStats->assertions );
                stream << "]\n" << std::endl;
            }
            AccumulatingReporter::sectionEnded( _sectionStats );
        }
        virtual void testCaseEnded( Ptr<TestCaseStats const> const& _testCaseStats ) {
            if( !unusedTestCaseInfo ) {
                stream << "[Summary for test case '" << _testCaseStats->testInfo.name << "': ";
                printTotals( _testCaseStats->totals );
                stream << "]\n" << std::endl;
            }
            AccumulatingReporter::testCaseEnded( _testCaseStats );
        }
        virtual void testGroupEnded( Ptr<TestGroupStats const> const& _testGroupStats ) {
            // !TBD
            AccumulatingReporter::testGroupEnded( _testGroupStats );
        }
        virtual void testRunEnded( Ptr<TestRunStats const> const& _testRunStats ) {
            // !TBD
            AccumulatingReporter::testRunEnded( _testRunStats );
        }

    };

    INTERNAL_CATCH_REGISTER_REPORTER( "console", ConsoleReporter )

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
