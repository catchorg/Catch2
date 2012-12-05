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
                stream << "[Group: " << unusedGroupInfo->name << "]" << std::endl;
            unusedGroupInfo.reset();
        }
        void lazyPrintTestCaseInfo() {
            stream << "[TestCase: " << unusedTestCaseInfo->name << "]" << std::endl;
            unusedTestCaseInfo.reset();
        }
        void lazyPrintSectionInfo() {
            ThreadedSectionInfo* section = unusedSectionInfo.get();
            bool firstSection = true;
            stream << "[Section: ";
            do {
                if( firstSection )
                    firstSection = false;
                else
                    stream << ",\n          ";
                stream << section->name;
                section = section->parent.get();
            }
            while( section );

            stream << "]" << std::endl;
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

        virtual void sectionEnded( Ptr<SectionStats const> const& _sectionStats ) {
            if( !unusedSectionInfo ) {
                stream << "[Section totals: ]" << std::endl;
            }
            AccumulatingReporter::sectionEnded( _sectionStats );
        }
        virtual void testCaseEnded( Ptr<TestCaseStats const> const& _testCaseStats ) {
            if( !unusedTestCaseInfo ) {
                stream << "[TestCase totals: ]\n" << std::endl;
            }
            AccumulatingReporter::testCaseEnded( _testCaseStats );
        }
        
    };

    INTERNAL_CATCH_REGISTER_REPORTER( "console", ConsoleReporter )

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
