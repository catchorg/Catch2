/*
 *  Created by Phil on 13/5/2013.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"
#include "catch_test_spec_parser.hpp"


inline Catch::TestCase fakeTestCase( const char* name, const char* desc = "" ){ return Catch::makeTestCase( NULL, "", name, desc, CATCH_INTERNAL_LINEINFO ); }

TEST_CASE( "Parse test names and tags", "" ) {

    using Catch::parseTestSpec;
    using Catch::TestSpec;

    Catch::TestCase tcA = fakeTestCase( "a", "" );
    Catch::TestCase tcB = fakeTestCase( "b", "[one][x]" );
    Catch::TestCase tcC = fakeTestCase( "longer name with spaces", "[two][three][.][x]" );
    Catch::TestCase tcD = fakeTestCase( "zlonger name with spacesz", "" );

    SECTION( "Empty test spec should have no filters", "" ) {
        TestSpec spec;
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
    }

    SECTION( "Test spec from empty string should have no filters", "" ) {
        TestSpec spec = parseTestSpec( "" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches(tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
    }

    SECTION( "Test spec from just a comma should have no filters", "" ) {
        TestSpec spec = parseTestSpec( "," );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
    }

    SECTION( "Test spec from name should have one filter", "" ) {
        TestSpec spec = parseTestSpec( "b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
    }

    SECTION( "Test spec from quoted name should have one filter", "" ) {
        TestSpec spec = parseTestSpec( "\"b\"" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
    }

    SECTION( "Test spec from name should have one filter", "" ) {
        TestSpec spec = parseTestSpec( "b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
    }

    SECTION( "Wildcard at the start", "" ) {
        TestSpec spec = parseTestSpec( "*spaces" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
        CHECK( parseTestSpec( "*a" ).matches( tcA ) == true );
    }
    SECTION( "Wildcard at the end", "" ) {
        TestSpec spec = parseTestSpec( "long*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
        CHECK( parseTestSpec( "a*" ).matches( tcA ) == true );
    }
    SECTION( "Wildcard at both ends", "" ) {
        TestSpec spec = parseTestSpec( "*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
        CHECK( parseTestSpec( "*a*" ).matches( tcA ) == true );
    }
    SECTION( "Redundant wildcard at the start", "" ) {
        TestSpec spec = parseTestSpec( "*a" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
    }
    SECTION( "Redundant wildcard at the end", "" ) {
        TestSpec spec = parseTestSpec( "a*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
    }
    SECTION( "Redundant wildcard at both ends", "" ) {
        TestSpec spec = parseTestSpec( "*a*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
    }
    SECTION( "Wildcard at both ends, redundant at start", "" ) {
        TestSpec spec = parseTestSpec( "*longer*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "Just wildcard", "" ) {
        TestSpec spec = parseTestSpec( "*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
    }

    SECTION( "Single tag", "" ) {
        TestSpec spec = parseTestSpec( "[one]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
    }
    SECTION( "Single tag, two matches", "" ) {
        TestSpec spec = parseTestSpec( "[x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "Two tags", "" ) {
        TestSpec spec = parseTestSpec( "[two][x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "Two tags, spare separated", "" ) {
        TestSpec spec = parseTestSpec( "[two] [x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "Wildcarded name and tag", "" ) {
        TestSpec spec = parseTestSpec( "*name*[x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "Single tag exclusion", "" ) {
        TestSpec spec = parseTestSpec( "~[one]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "One tag exclusion and one tag inclusion", "" ) {
        TestSpec spec = parseTestSpec( "~[two][x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
    }
    SECTION( "One tag exclusion and one wldcarded name inclusion", "" ) {
        TestSpec spec = parseTestSpec( "~[two]*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "One tag exclusion, using exclude:, and one wldcarded name inclusion", "" ) {
        TestSpec spec = parseTestSpec( "exclude:[two]*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "name exclusion", "" ) {
        TestSpec spec = parseTestSpec( "~b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "wildcarded name exclusion", "" ) {
        TestSpec spec = parseTestSpec( "~*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "wildcarded name exclusion with tag inclusion", "" ) {
        TestSpec spec = parseTestSpec( "~*name*,[three]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "wildcarded name exclusion, using exclude:, with tag inclusion", "" ) {
        TestSpec spec = parseTestSpec( "exclude:*name*,[three]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "two wildcarded names", "" ) {
        TestSpec spec = parseTestSpec( "\"longer*\"\"*spaces\"" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "empty tag", "" ) {
        TestSpec spec = parseTestSpec( "[]" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "empty quoted name", "" ) {
        TestSpec spec = parseTestSpec( "\"\"" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "quoted string followed by tag exclusion", "" ) {
        TestSpec spec = parseTestSpec( "\"*name*\"~[.]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == true );
    }

}
