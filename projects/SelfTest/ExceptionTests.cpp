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

#include "catch_self_test.hpp"

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

TEST_CASE( "./succeeding/exceptions/explicit", "When checked exceptions are thrown they can be expected or unexpected" )
{
    REQUIRE_THROWS_AS( thisThrows(), std::domain_error );
    REQUIRE_NOTHROW( thisDoesntThrow() );
    REQUIRE_THROWS( thisThrows() );
}

TEST_CASE( "./failing/exceptions/explicit", "When checked exceptions are thrown they can be expected or unexpected" )
{
    CHECK_THROWS_AS( thisThrows(), std::string );
    CHECK_THROWS_AS( thisDoesntThrow(), std::domain_error );
    CHECK_NOTHROW( thisThrows() );
}

TEST_CASE( "./failing/exceptions/implicit", "When unchecked exceptions are thrown they are always failures" )
{
	if( Catch::isTrue( true ) )
	    throw std::domain_error( "unexpected exception" );
}

TEST_CASE( "./failing/exceptions/implicit/2", "An unchecked exception reports the line of the last assertion" )
{
    CHECK( 1 == 1 );
	if( Catch::isTrue( true ) )
	    throw std::domain_error( "unexpected exception" );
}
TEST_CASE( "./failing/exceptions/implicit/3", "When unchecked exceptions are thrown they are always failures" )
{
    SECTION( "section name", "" )
    {
		if( Catch::isTrue( true ) )
			throw std::domain_error( "unexpected exception" );
    }
}

TEST_CASE( "./failing/exceptions/implicit/4", "When unchecked exceptions are thrown they are always failures" )
{
    CHECK( thisThrows() == 0 );
}

TEST_CASE( "./succeeding/exceptions/implicit", "When unchecked exceptions are thrown, but caught, they do not affect the test" )
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

TEST_CASE( "./failing/exceptions/custom", "Unexpected custom exceptions can be translated" )
{
	if( Catch::isTrue( true ) )
	    throw CustomException( "custom exception" );
}

inline void throwCustom() {
	if( Catch::isTrue( true ) )
		throw CustomException( "custom exception - not std" );
}

TEST_CASE( "./failing/exceptions/custom/nothrow", "Custom exceptions can be translated when testing for nothrow" )
{
    REQUIRE_NOTHROW( throwCustom() );
}

TEST_CASE( "./failing/exceptions/custom/throw", "Custom exceptions can be translated when testing for throwing as something else" )
{
    REQUIRE_THROWS_AS( throwCustom(), std::exception );
}


TEST_CASE( "./failing/exceptions/custom/double", "Unexpected custom exceptions can be translated"  )
{
	if( Catch::isTrue( true ) )
	    throw double( 3.14 );
}

inline int thisFunctionNotImplemented( int ) {
    CATCH_NOT_IMPLEMENTED;
}

TEST_CASE( "./succeeding/exceptions/notimplemented", "" )
{
    REQUIRE_THROWS( thisFunctionNotImplemented( 7 ) );
}
