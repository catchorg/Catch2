/*
 *  Created by Phil on 1/10/2015.
 *  Copyright 2015 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "internal/catch_suppress_warnings.h"
#include "internal/catch_test_case_tracker.hpp"


namespace Catch
{
    class LocalContext {

    public:
        TrackerContext& operator()() const {
            return TrackerContext::instance();
        }
    };

} // namespace Catch

inline Catch::TrackerContext& C_A_T_C_H_Context() {
    return Catch::TrackerContext::instance();
}

// -------------------

#include "catch.hpp"

using namespace Catch;

//inline void testCase( Catch::LocalContext const& C_A_T_C_H_Context ) {
//
//    REQUIRE( C_A_T_C_H_Context().i() == 42 );
//}

Catch::TestCaseTracking::NameAndLocation makeNAL( std::string const& name ) {
    return Catch::TestCaseTracking::NameAndLocation( name, Catch::SourceLineInfo() );
}

TEST_CASE( "Tracker", "" ) {

    TrackerContext ctx;
    ctx.startRun();
    ctx.startCycle();


    ITracker& testCase = SectionTracker::acquire( ctx, makeNAL( "Testcase" ) );
    REQUIRE( testCase.isOpen() );

    ITracker& s1 = SectionTracker::acquire( ctx, makeNAL( "S1" ) );
    REQUIRE( s1.isOpen() );

    SECTION( "successfully close one section", "" ) {
        s1.close();
        REQUIRE( s1.isSuccessfullyCompleted() );
        REQUIRE( testCase.isComplete() == false );

        testCase.close();
        REQUIRE( ctx.completedCycle() );
        REQUIRE( testCase.isSuccessfullyCompleted() );
    }

    SECTION( "fail one section", "" ) {
        s1.fail();
        REQUIRE( s1.isComplete() );
        REQUIRE( s1.isSuccessfullyCompleted() == false );
        REQUIRE( testCase.isComplete() == false );

        testCase.close();
        REQUIRE( ctx.completedCycle() );
        REQUIRE( testCase.isSuccessfullyCompleted() == false );

        SECTION( "re-enter after failed section", "" ) {
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
        SECTION( "re-enter after failed section and find next section", "" ) {
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

    SECTION( "successfully close one section, then find another", "" ) {
        s1.close();

        ITracker& s2 = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
        REQUIRE( s2.isOpen() == false );

        testCase.close();
        REQUIRE( testCase.isComplete() == false );

        SECTION( "Re-enter - skips S1 and enters S2", "" ) {
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

    SECTION( "open a nested section", "" ) {
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

    SECTION( "start a generator", "" ) {
        IndexTracker& g1 = IndexTracker::acquire( ctx, makeNAL( "G1" ), 2 );
        REQUIRE( g1.isOpen() );
        REQUIRE( g1.index() == 0 );

        REQUIRE( g1.isComplete() == false );
        REQUIRE( s1.isComplete() == false );

        SECTION( "close outer section" )
        {
            s1.close();
            REQUIRE( s1.isComplete() == false );
            testCase.close();
            REQUIRE( testCase.isSuccessfullyCompleted() == false );

            SECTION( "Re-enter for second generation", "" ) {
                ctx.startCycle();
                ITracker& testCase2 = SectionTracker::acquire( ctx, makeNAL( "Testcase" ) );
                REQUIRE( testCase2.isOpen() );

                ITracker& s1b = SectionTracker::acquire( ctx, makeNAL( "S1" ) );
                REQUIRE( s1b.isOpen() );


                IndexTracker& g1b = IndexTracker::acquire( ctx, makeNAL( "G1" ), 2 );
                REQUIRE( g1b.isOpen() );
                REQUIRE( g1b.index() == 1 );

                REQUIRE( s1.isComplete() == false );

                s1b.close();
                REQUIRE( s1b.isComplete() );
                REQUIRE( g1b.isComplete() );
                testCase2.close();
                REQUIRE( testCase2.isComplete() );
            }
        }
        SECTION( "Start a new inner section", "" ) {
            ITracker& s2 = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
            REQUIRE( s2.isOpen() );

            s2.close();
            REQUIRE( s2.isComplete() );

            s1.close();
            REQUIRE( s1.isComplete() == false );

            testCase.close();
            REQUIRE( testCase.isComplete() == false );

            SECTION( "Re-enter for second generation", "" ) {
                ctx.startCycle();
                ITracker& testCase2 = SectionTracker::acquire( ctx, makeNAL( "Testcase" ) );
                REQUIRE( testCase2.isOpen() );

                ITracker& s1b = SectionTracker::acquire( ctx, makeNAL( "S1" ) );
                REQUIRE( s1b.isOpen() );

                // generator - next value
                IndexTracker& g1b = IndexTracker::acquire( ctx, makeNAL( "G1" ), 2 );
                REQUIRE( g1b.isOpen() );
                REQUIRE( g1b.index() == 1 );

                // inner section again
                ITracker& s2b = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
                REQUIRE( s2b.isOpen() );

                s2b.close();
                REQUIRE( s2b.isComplete() );

                s1b.close();
                REQUIRE( g1b.isComplete() );
                REQUIRE( s1b.isComplete() );

                testCase2.close();
                REQUIRE( testCase2.isComplete() );
            }
        }

        SECTION( "Fail an inner section", "" ) {
            ITracker& s2 = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
            REQUIRE( s2.isOpen() );

            s2.fail();
            REQUIRE( s2.isComplete() );
            REQUIRE( s2.isSuccessfullyCompleted() == false );

            s1.close();
            REQUIRE( s1.isComplete() == false );

            testCase.close();
            REQUIRE( testCase.isComplete() == false );

            SECTION( "Re-enter for second generation", "" ) {
                ctx.startCycle();
                ITracker& testCase2 = SectionTracker::acquire( ctx, makeNAL( "Testcase" ) );
                REQUIRE( testCase2.isOpen() );

                ITracker& s1b = SectionTracker::acquire( ctx, makeNAL( "S1" ) );
                REQUIRE( s1b.isOpen() );

                // generator - still same value
                IndexTracker& g1b = IndexTracker::acquire( ctx, makeNAL( "G1" ), 2 );
                REQUIRE( g1b.isOpen() );
                REQUIRE( g1b.index() == 0 );

                // inner section again - this time won't open
                ITracker& s2b = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
                REQUIRE( s2b.isOpen() == false );

                s1b.close();
                REQUIRE( g1b.isComplete() == false );
                REQUIRE( s1b.isComplete() == false );

                testCase2.close();
                REQUIRE( testCase2.isComplete() == false );

                // Another cycle - now should complete
                ctx.startCycle();
                ITracker& testCase3 = SectionTracker::acquire( ctx, makeNAL( "Testcase" ) );
                REQUIRE( testCase3.isOpen() );

                ITracker& s1c = SectionTracker::acquire( ctx, makeNAL( "S1" ) );
                REQUIRE( s1c.isOpen() );

                // generator - now next value
                IndexTracker& g1c = IndexTracker::acquire( ctx, makeNAL( "G1" ), 2 );
                REQUIRE( g1c.isOpen() );
                REQUIRE( g1c.index() == 1 );

                // inner section - now should open again
                ITracker& s2c = SectionTracker::acquire( ctx, makeNAL( "S2" ) );
                REQUIRE( s2c.isOpen() );

                s2c.close();
                REQUIRE( s2c.isComplete() );

                s1c.close();
                REQUIRE( g1c.isComplete() );
                REQUIRE( s1c.isComplete() );

                testCase3.close();
                REQUIRE( testCase3.isComplete() );
            }
        }
        // !TBD"
        //   nested generator
        //   two sections within a generator
    }
}
