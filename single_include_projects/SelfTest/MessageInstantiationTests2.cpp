/*
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"
#define INTERNAL_CATCH_INLINE inline
#include "../../include/internal/catch_message.hpp"

namespace Counter {
extern int g_haveCountedMessages;
}

namespace MI2
{
    // This test works with the equivalent in MessageInstantiationTests1.cpp
    // The first test to reach this point will increment the MessageInfo counter.  Subsequent tests
    // just check the value.  The purpose of this test is to verify that the compiler's
    // greedy instantiation (or whatever process it uses) eliminate all other
    // references to the globalCount

    TEST_CASE("message counting2","[vs]")
    {
        if( Counter::g_haveCountedMessages > 0 ) {
            REQUIRE( Catch::MessageInfoCounter<unsigned int>::globalCount > 0 );
        }
        else
        {
            ++Catch::MessageInfoCounter<unsigned int>::globalCount;
            Counter::g_haveCountedMessages = 1;
        }
    }
}

namespace LongCounter {
extern int g_haveCountedMessagesLong;
}

namespace MI2
{
    TEST_CASE("long message counting2","[vs]")
    {
        if( LongCounter::g_haveCountedMessagesLong > 0 ) {
            REQUIRE( Catch::MessageInfoCounter<long>::globalCount > 0 );
        }
        else
        {
            ++Catch::MessageInfoCounter<long>::globalCount;
            LongCounter::g_haveCountedMessagesLong = 1;
        }
    }
}
