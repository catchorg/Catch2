/*
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"
#include <iostream>

TEST_CASE( "INFO and WARN do not abort tests", "[messages][.]" ) {
    INFO( "this is a " << "message" );    // This should output the message if a failure occurs
    WARN( "this is a " << "warning" );    // This should always output the message but then continue
}

TEST_CASE( "SUCCEED counts as a test pass", "[messages]" ) {
    SUCCEED( "this is a " << "success" );
}

TEST_CASE( "INFO gets logged on failure", "[failing][messages][.]" ) {
    INFO( "this message should be logged" );
    INFO( "so should this" );
    int a = 2;
    REQUIRE( a == 1 );
}

TEST_CASE( "INFO gets logged on failure, even if captured before successful assertions", "[failing][messages][.]" ) {
    INFO( "this message may be logged later" );
    int a = 2;
    CHECK( a == 2 );

    INFO( "this message should be logged" );

    CHECK( a == 1 );

    INFO( "and this, but later" );

    CHECK( a == 0 );

    INFO( "but not this" );

    CHECK( a == 2 );
}

TEST_CASE( "FAIL aborts the test", "[failing][messages][.]" ) {
    FAIL( "This is a " << "failure" );    // This should output the message and abort
    WARN( "We should never see this");
}

TEST_CASE( "FAIL_CHECK does not abort the test", "[failing][messages][.]" ) {
    FAIL_CHECK( "This is a " << "failure" );    // This should output the message then continue
    WARN( "This message appears in the output");
}

TEST_CASE( "FAIL does not require an argument", "[failing][messages][.]" ) {
    FAIL();
}

TEST_CASE( "SUCCEED does not require an argument", "[messages][.]" ) {
   SUCCEED();
}

TEST_CASE( "Output from all sections is reported", "[failing][messages][.]" ) {
    SECTION( "one" ) {
        FAIL( "Message from section one" );
    }

    SECTION( "two" ) {
        FAIL( "Message from section two" );
    }
}

TEST_CASE( "Standard output from all sections is reported", "[messages][.]" ) {
    SECTION( "one" ) {
        std::cout << "Message from section one" << std::endl;
    }

    SECTION( "two" ) {
        std::cout << "Message from section two" << std::endl;
    }
}

TEST_CASE( "Standard error is reported and redirected", "[messages][.][approvals]" ) {
    SECTION( "std::cerr" ) {
        std::cerr << "Write to std::cerr" << std::endl;
    }
    SECTION( "std::clog" ) {
        std::clog << "Write to std::clog" << std::endl;
    }
    SECTION( "Interleaved writes to cerr and clog" ) {
        std::cerr << "Inter";
        std::clog << "leaved";
        std::cerr << ' ';
        std::clog << "writes";
        std::cerr << " to error";
        std::clog << " streams" << std::endl;
    }
}

TEST_CASE( "INFO is reset for each loop", "[messages][failing][.]" ) {
    for( int i=0; i<100; i++ )
    {
        INFO( "current counter " << i );
        CAPTURE( i );
        REQUIRE( i < 10 );
    }
}

TEST_CASE( "The NO_FAIL macro reports a failure but does not fail the test", "[messages]" ) {
    CHECK_NOFAIL( 1 == 2 );
}

TEST_CASE( "just info", "[info][isolated info][messages]" ) {
    INFO( "this should never be seen" );
}
TEST_CASE( "just failure", "[fail][isolated info][.][messages]" ) {
    FAIL( "Previous info should not be seen" );
}


TEST_CASE( "sends information to INFO", "[.][failing]" ) {
    INFO( "hi" );
    int i = 7;
    CAPTURE( i );
    REQUIRE( false );
}

TEST_CASE( "Pointers can be converted to strings", "[messages][.][approvals]" ) {
    int p;
    WARN( "actual address of p: " << &p );
    WARN( "toString(p): " << ::Catch::Detail::stringify( &p ) );
}

TEST_CASE( "CAPTURE can deal with complex expressions", "[messages][capture]" ) {
    int a = 1;
    int b = 2;
    int c = 3;
    CAPTURE( a, b, c, a + b, a+b, c > b, a == 1 );
    SUCCEED();
}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value" // In (1, 2), the "1" is unused ...
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value" // All the comma operators are side-effect free
#endif
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4709) // comma in indexing operator
#endif

template <typename T1, typename T2>
struct helper_1436 {
    helper_1436(T1 t1, T2 t2):
        t1{ t1 },
        t2{ t2 }
    {}
    T1 t1;
    T2 t2;
};

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& out, helper_1436<T1, T2> const& helper) {
    out << "{ " << helper.t1 << ", " << helper.t2 << " }";
    return out;
}

TEST_CASE("CAPTURE can deal with complex expressions involving commas", "[messages][capture]") {
    CAPTURE(std::vector<int>{1, 2, 3}[0, 1, 2],
            std::vector<int>{1, 2, 3}[(0, 1)],
            std::vector<int>{1, 2, 3}[0]);
    CAPTURE((helper_1436<int, int>{12, -12}),
            (helper_1436<int, int>(-12, 12)));
    CAPTURE( (1, 2), (2, 3) );
    SUCCEED();
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif
