/*
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#include <string>
#include <stdexcept>

namespace
{
    inline int thisThrows()
    {
		if( Catch::isTrue( true ) )
		    throw std::domain_error( "expected exception" );
		return 1;
    }

    int thisDoesntThrow()
    {
        return 0;
    }
}

namespace ExceptionTests
{
TEST_CASE( "When checked exceptions are thrown they can be expected or unexpected", "" )
{
    REQUIRE_THROWS_AS( thisThrows(), std::domain_error );
    REQUIRE_NOTHROW( thisDoesntThrow() );
    REQUIRE_THROWS( thisThrows() );
}

TEST_CASE( "Expected exceptions that don't throw or unexpected exceptions fail the test", "[.][failing]" )
{
    CHECK_THROWS_AS( thisThrows(), std::string );
    CHECK_THROWS_AS( thisDoesntThrow(), std::domain_error );
    CHECK_NOTHROW( thisThrows() );
}

TEST_CASE( "When unchecked exceptions are thrown directly they are always failures", "[.][failing]" )
{
	if( Catch::isTrue( true ) )
	    throw std::domain_error( "unexpected exception" );
}

TEST_CASE( "An unchecked exception reports the line of the last assertion", "[.][failing]" )
{
    CHECK( 1 == 1 );
	if( Catch::isTrue( true ) )
	    throw std::domain_error( "unexpected exception" );
}
TEST_CASE( "When unchecked exceptions are thrown from sections they are always failures", "[.][failing]" )
{
    SECTION( "section name", "" )
    {
		if( Catch::isTrue( true ) )
			throw std::domain_error( "unexpected exception" );
    }
}

TEST_CASE( "When unchecked exceptions are thrown from functions they are always failures", "[.][failing]" )
{
    CHECK( thisThrows() == 0 );
}

TEST_CASE( "When unchecked exceptions are thrown during a REQUIRE the test should abort fail", "[.][failing]" )
{
    REQUIRE( thisThrows() == 0 );
    FAIL( "This should never happen" );
}

TEST_CASE( "When unchecked exceptions are thrown during a CHECK the test should abort and fail", "[.][failing]" )
{
    CHECK( thisThrows() == 0 );
    FAIL( "This should never happen" );
}

TEST_CASE( "When unchecked exceptions are thrown, but caught, they do not affect the test", "" )
{
    try
    {
        throw std::domain_error( "unexpected exception" );
    }
    catch(...)
    {
    }
}

class CustomException
{
public:
    CustomException( const std::string& msg )
    : m_msg( msg )
    {}
    
    std::string getMessage() const
    {
        return m_msg;
    }
    
private:
    std::string m_msg;
};

CATCH_TRANSLATE_EXCEPTION( CustomException& ex )
{
    return ex.getMessage();
}

CATCH_TRANSLATE_EXCEPTION( double& ex )
{
    return Catch::toString( ex );
}

TEST_CASE("Unexpected custom exceptions can be translated", "[.][failing]" )
{
	if( Catch::isTrue( true ) )
	    throw CustomException( "custom exception" );
}

inline void throwCustom() {
	if( Catch::isTrue( true ) )
		throw CustomException( "custom exception - not std" );
}

TEST_CASE( "Custom exceptions can be translated when testing for nothrow", "[.][failing]" )
{
    REQUIRE_NOTHROW( throwCustom() );
}

TEST_CASE( "Custom exceptions can be translated when testing for throwing as something else", "[.][failing]" )
{
    REQUIRE_THROWS_AS( throwCustom(), std::exception );
}


TEST_CASE( "Unexpected exceptions can be translated", "[.][failing]"  )
{
	if( Catch::isTrue( true ) )
	    throw double( 3.14 );
}

inline int thisFunctionNotImplemented( int ) {
    CATCH_NOT_IMPLEMENTED;
}

TEST_CASE( "NotImplemented exception", "" )
{
    REQUIRE_THROWS( thisFunctionNotImplemented( 7 ) );
}
}
