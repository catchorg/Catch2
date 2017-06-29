#include "internal/catch_stringbuilder.h"
#include "../include/internal/catch_stringref.h"
#include "../include/internal/catch_string.h"

#include "catch.hpp"

TEST_CASE( "StringBuilder", "[Strings]" ) {
    
    using Catch::StringBuilder;
    using Catch::String;
    
    StringBuilder sb;
    
    SECTION( "basic" ) {
        REQUIRE( sb.capacity() == 0 );
        REQUIRE( sb.size() == 0 );

        sb.reserve( 32 );
        REQUIRE( sb.capacity() == 32 );
        REQUIRE( sb.size() == 0 );
        
        sb.append( "hello" );
        REQUIRE( sb.capacity() == 32 );
        REQUIRE( sb.size() == 5 );
        
        String s = std::move( sb );
        REQUIRE( s == "hello" );
        REQUIRE( s.size() == 5 );
    }
    
    SECTION( "concatenation" ) {
        sb << "hello" << " " << "world";
        String s = std::move( sb );
        REQUIRE( s == "hello world" );
    }

    SECTION( "concat & move" ) {
        String s = StringBuilder() << "hello" << " " << "world";
        REQUIRE( s == "hello world" );
    }

    SECTION( "reserved" ) {
        StringBuilder sb16( 16 );
        REQUIRE( sb16.capacity() == 16 );
        sb16 << "hello" << " " << "world";
        REQUIRE( sb16.capacity() == 16 );
        String s = std::move( sb16 );
        REQUIRE( s == "hello world" );
    }

    SECTION( "from String" ) {
        String s = "hello";

        SECTION( "copy" ) {
            StringBuilder sb2 = s;
            String s2( std::move(sb2) );
            REQUIRE( s2 == s );
            REQUIRE( s2.c_str() != s.c_str() );
        }
        SECTION( "move from uniquely owned string" ) {
            auto originalPointer = s.c_str();
            StringBuilder sb2( std::move( s ) );
            String s2( std::move(sb2) );
            REQUIRE( s2 == "hello" );
            REQUIRE( s2.c_str() == originalPointer );
        }
        SECTION( "move from shared string (copies)" ) {
            auto originalPointer = s.c_str();
            String keepAlive = s;
            StringBuilder sb2( std::move( s ) );
            String s2( std::move(sb2) );
            REQUIRE( s2 == "hello" );
            REQUIRE( s2.c_str() != originalPointer );
        }
    }
}
