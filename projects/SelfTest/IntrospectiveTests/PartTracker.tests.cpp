/*
 *  Created by Phil on 1/10/2015.
 *  Copyright 2015 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "internal/catch_suppress_warnings.h"
#include "internal/catch_test_case_tracker.h"


namespace Catch
{
    class LocalContext {

    public:
        TrackerContext& operator()() const {
            return TrackerContext::instance();
        }
    };

} // namespace Catch

// -------------------

#include "catch.hpp"

using namespace Catch;

namespace {
Catch::TestCaseTracking::NameAndLocation makeNAL( std::string const& name ) {
    return Catch::TestCaseTracking::NameAndLocation( name, Catch::SourceLineInfo("",0) );
}
}

TEST_CASE( "Tracker" ) {

    TrackerContext ctx;
    ctx.startRun();
    ctx.startCycle();


    ITracker& testCase = SectionTracker::acquire( ctx, makeNAL( "Testcase" ) );
    REQUIRE( testCase.isOpen() );

    ITracker& s1 = SectionTracker::acquire( ctx, makeNAL( "S1" ) );
    REQUIRE( s1.isOpen() );

    SECTION( "successfully close one section" ) {
        s1.close();
        REQUIRE( s1.isSuccessfullyCompleted() );
        REQUIRE( testCase.isComplete() == false );

        testCase.close();
        REQUIRE( ctx.completedCycle() );
        REQUIRE( testCase.isSuccessfullyCompleted() );
    }

    SECTION( "fail one section" ) {
        s1.fail();
        REQUIRE( s1.isComplete() );
        REQUIRE( s1.isSuccessfullyCompleted() == false );
        REQUIRE( testCase.isComplete() == false );

        testCase.close();
        REQUIRE( ctx.completedCycle() );
        REQUIRE( testCase.isSuccessfullyCompleted() == false );

        SECTION( "re-enter after failed section" ) {
            ctx.startCycle();
            ITracker& testCase2 = SectionTracker::acquire( ctx, makeNAL( "Testcase" ) );
            REQUIRE( testCase2.isOpen() );

            ITracker& s1b = SectionTracker::acquire( ctx, makeNAL( "S1" ) );
            REQUIRE( s1b.isOpen() == false );

            testCase2.close();
            REQUIRE( ctx.completedCycle() );
            REQUIRE( testCase.isComplete() );
            REQUIRE( testCase.isSuccessfullyCompleted() );
        }
        SECTION( "re-enter after failed section and find next section" ) {
            ctx.startCycle();
            ITracker& testCase2 = SectionTracker::acquire( ctx, makeNAL( "Testcase" ) );
            REQUIRE( testCase2.isOpen() );

            ITracker& s1b = SectionTracker::acquire( ctx, makeNAL( "S1" ) );
            REQUIRE( s1b.isOpen() == false );

            ITracker& s2 = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
            REQUIRE( s2.isOpen() );

            s2.close();
            REQUIRE( ctx.completedCycle() );

            testCase2.close();
            REQUIRE( testCase.isComplete() );
            REQUIRE( testCase.isSuccessfullyCompleted() );
        }
    }

    SECTION( "successfully close one section, then find another" ) {
        s1.close();

        ITracker& s2 = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
        REQUIRE( s2.isOpen() == false );

        testCase.close();
        REQUIRE( testCase.isComplete() == false );

        SECTION( "Re-enter - skips S1 and enters S2" ) {
            ctx.startCycle();
            ITracker& testCase2 = SectionTracker::acquire( ctx, makeNAL( "Testcase" ) );
            REQUIRE( testCase2.isOpen() );

            ITracker& s1b = SectionTracker::acquire( ctx, makeNAL( "S1" ) );
            REQUIRE( s1b.isOpen() == false );

            ITracker& s2b = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
            REQUIRE( s2b.isOpen() );

            REQUIRE( ctx.completedCycle() == false );

            SECTION ("Successfully close S2") {
                s2b.close();
                REQUIRE( ctx.completedCycle() );

                REQUIRE( s2b.isSuccessfullyCompleted() );
                REQUIRE( testCase2.isComplete() == false );

                testCase2.close();
                REQUIRE( testCase2.isSuccessfullyCompleted() );
            }
            SECTION ("fail S2") {
                s2b.fail();
                REQUIRE( ctx.completedCycle() );

                REQUIRE( s2b.isComplete() );
                REQUIRE( s2b.isSuccessfullyCompleted() == false );

                testCase2.close();
                REQUIRE( testCase2.isSuccessfullyCompleted() == false );

                // Need a final cycle
                ctx.startCycle();
                ITracker& testCase3 = SectionTracker::acquire( ctx, makeNAL( "Testcase" ) );
                REQUIRE( testCase3.isOpen() );

                ITracker& s1c = SectionTracker::acquire( ctx, makeNAL( "S1" ) );
                REQUIRE( s1c.isOpen() == false );

                ITracker& s2c = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
                REQUIRE( s2c.isOpen() == false );

                testCase3.close();
                REQUIRE( testCase3.isSuccessfullyCompleted() );
            }
        }
    }

    SECTION( "open a nested section" ) {
        ITracker& s2 = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
        REQUIRE( s2.isOpen() );

        s2.close();
        REQUIRE( s2.isComplete() );
        REQUIRE( s1.isComplete() == false );

        s1.close();
        REQUIRE( s1.isComplete() );
        REQUIRE( testCase.isComplete() == false );

        testCase.close();
        REQUIRE( testCase.isComplete() );
    }
}

static bool previouslyRun = false;
static bool previouslyRunNested = false;

TEST_CASE( "#1394", "[.][approvals][tracker]" ) {
    // -- Don't re-run after specified section is done
    REQUIRE(previouslyRun == false);

    SECTION( "RunSection" ) {
        previouslyRun = true;
    }
    SECTION( "SkipSection" ) {
        // cause an error if this section is called because it shouldn't be
        REQUIRE(1 == 0);
    }
}

TEST_CASE( "#1394 nested", "[.][approvals][tracker]" ) {
    REQUIRE(previouslyRunNested == false);

    SECTION( "NestedRunSection" ) {
        SECTION( "s1" ) {
            previouslyRunNested = true;
        }
    }
    SECTION( "NestedSkipSection" ) {
        // cause an error if this section is called because it shouldn't be
        REQUIRE(1 == 0);
    }
}
