#include "../include/internal/catch_string.h"

#include "catch.hpp"

#include <cstring>

TEST_CASE( "String", "[Strings]" ) {
    using Catch::String;
    
    SECTION( "empty string" ) {
        String empty;
        REQUIRE( empty.empty() );
        REQUIRE( empty.size() == 0 );
        REQUIRE( std::strcmp( empty.c_str(), "" ) == 0 );
    }
    SECTION( "from literal" ) {
        String s = "hello";
        REQUIRE( s.empty() == false );
        REQUIRE( s.size() == 5 );
    }
    
}