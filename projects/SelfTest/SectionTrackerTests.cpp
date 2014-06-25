/*
 *  Created by Phil on 20/07/2013.
 *  Copyright 2013 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "internal/catch_test_case_tracker.hpp"

#include "catch.hpp"

TEST_CASE( "section tracking", "" ) {

    using namespace Catch;
    TestCaseTracker testCaseTracker( "test case" );

    const std::string section1Name = "section 1";
    const std::string section2Name = "section 2";

    CHECK_FALSE( testCaseTracker.isCompleted() );

    SECTION( "test case with no sections", "" ) {

        {
            TestCaseTracker::Guard guard( testCaseTracker );
            CHECK_FALSE( testCaseTracker.isCompleted() );
        }
        CHECK( testCaseTracker.isCompleted() );
    }

    SECTION( "test case with one section", "" ) {

        {
            TestCaseTracker::Guard guard( testCaseTracker );

            // Enter section? - yes
            CHECK( testCaseTracker.enterSection( section1Name ) );
            CHECK_FALSE( testCaseTracker.isCompleted() );
            testCaseTracker.leaveSection();

            // Leave test case - now complete
        }
        CHECK( testCaseTracker.isCompleted() );

        // ...

        // Enter test case again
        {
            TestCaseTracker::Guard guard( testCaseTracker );

            // Enter section? - no - now complete
            CHECK_FALSE( testCaseTracker.enterSection( section1Name ) );
        }
    }

    SECTION( "test case with two consecutive sections", "" ) {

        // Enter test case
        {
            TestCaseTracker::Guard guard( testCaseTracker );

            // Enter section 1? - yes
            CHECK( testCaseTracker.enterSection( section1Name ) );
            testCaseTracker.leaveSection();

            // Enter section 2? - no - we just exected section 1
            CHECK_FALSE( testCaseTracker.enterSection( section2Name ) );

            // Leave test case - incomplete (still need to visit section 2)
        }
        CHECK_FALSE( testCaseTracker.isCompleted() );

        // ...

        // Enter test case again
        {
            TestCaseTracker::Guard guard( testCaseTracker );

            // Enter section 1? - no, already done now
            CHECK_FALSE( testCaseTracker.enterSection( section1Name ) );

            // Enter section 2? - yes
            CHECK( testCaseTracker.enterSection( section2Name ) );
            testCaseTracker.leaveSection();

            // Leave test case - now complete
        }
        CHECK( testCaseTracker.isCompleted() );        
    }

    SECTION( "test case with one section within another", "" ) {

        // Enter test case again
        {
            TestCaseTracker::Guard guard( testCaseTracker );

            // Enter section 1? - yes
            CHECK( testCaseTracker.enterSection( section1Name ) );

            // Enter section 2? - yes
            CHECK( testCaseTracker.enterSection( section2Name ) );

            CHECK_FALSE( testCaseTracker.isCompleted() );

            testCaseTracker.leaveSection(); // section 2
            testCaseTracker.leaveSection(); // section 1

            // Leave test case - now complete
        }
        CHECK( testCaseTracker.isCompleted() );        
    }
}
