/*
 *  Created by Phil on 5/12/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_CONSOLE_H_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_CONSOLE_H_INCLUDED

#include "catch_reporter_bases.hpp"
#include "internal/catch_timer.h"

#include <unordered_set>

#if defined( _MSC_VER )
#    pragma warning( push )
#    pragma warning( \
        disable : 4061 ) // Not all labels are EXPLICITLY handled in switch
                         // Note that 4062 (not all labels are handled
                         // and default is missing) is enabled
#endif

namespace Catch {
    struct GTestReporter : StreamingReporterBase<GTestReporter> {
        GTestReporter( ReporterConfig const& config );
        ~GTestReporter() override;
        static std::string getDescription();
        static std::set<Verbosity> getSupportedVerbosities();

        void noMatchingTestCases( std::string const& spec ) override;

        void reportInvalidArguments( std::string const& arg ) override;

        void testRunStarting( TestRunInfo const& _testRunInfo ) override;
        void testCaseStarting( TestCaseInfo const& _testInfo ) override;
        void sectionStarting( SectionInfo const& _sectionInfo ) override;
        void assertionStarting( AssertionInfo const& ) override;

        bool assertionEnded( AssertionStats const& _assertionStats ) override;
        void sectionEnded( SectionStats const& _sectionStats ) override;
        void testCaseEnded( TestCaseStats const& _testCaseStats ) override;
        void testRunEnded( TestRunStats const& _testRunStats ) override;

    private:
        bool shouldPrintRootSectionOnlyOnce() const;
        std::string formatFullSectionName();

        void printTestFilters();

        static std::string formatDuration( double seconds ) ;        

    private:
        struct Stats {
            Timer timer;
            // By "test" here we mean one pass through a test case. Each leaf
            // section corresponds to a test.
            Counts tests;
        };
        Stats m_runStats;
        Stats m_testCaseStats;
        std::size_t m_prevNestedSectionAssertionFailures;
        std::vector<std::string> m_failedSections;
        std::unordered_set<std::string> m_failedSectionsSet;
    };

} // end namespace Catch

#if defined( _MSC_VER )
#    pragma warning( pop )
#endif

#endif // TWOBLUECUBES_CATCH_REPORTER_CONSOLE_H_INCLUDED