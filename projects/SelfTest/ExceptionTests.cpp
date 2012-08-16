/*
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "catch.hpp"

#include <string>
#include <stdexcept>

#include "catch_self_test.hpp"

namespace
{
    CATCH_ATTRIBUTE_NORETURN
    int thisThrows();
    
    int thisThrows()
    {
        throw std::domain_error( "expected exception" );
        /*NOTREACHED*/    
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

CATCH_ATTRIBUTE_NORETURN
TEST_CASE( "./failing/exceptions/explicit", "When checked exceptions are thrown they can be expected or unexpected" )
{
    CHECK_THROWS_AS( thisThrows(), std::string );
    CHECK_THROWS_AS( thisDoesntThrow(), std::domain_error );
    CHECK_NOTHROW( thisThrows() );
}

TEST_CASE_NORETURN( "./failing/exceptions/implicit", "When unchecked exceptions are thrown they are always failures" )
{
    throw std::domain_error( "unexpected exception" );
    /*NOTREACHED*/    
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

TEST_CASE_NORETURN( "./failing/exceptions/custom", "Unexpected custom exceptions can be translated" )
{
    throw CustomException( "custom exception" );
}

TEST_CASE( "./failing/exceptions/custom/nothrow", "Custom exceptions can be translated when testing for nothrow" )
{
    REQUIRE_NOTHROW( throw CustomException( "unexpected custom exception" ) );
}

TEST_CASE( "./failing/exceptions/custom/throw", "Custom exceptions can be translated when testing for throwing as something else" )
{
    REQUIRE_THROWS_AS( throw CustomException( "custom exception - not std" ), std::exception );
}


TEST_CASE_NORETURN( "./failing/exceptions/custom/double", "Unexpected custom exceptions can be translated"  )
{
    throw double( 3.14 );
}

#ifdef  __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

TEST_CASE( "./failing/exceptions/in-section", "Exceptions thrown from sections report file/ line or section" )
{
    SECTION( "the section", "" )
    {
        CATCH_REGISTER_LINE_INFO( "the section2" ) SECTION( "the section2", "" )
        {
            throw std::domain_error( "Exception from section" );
        }
    }
}

#ifdef  __GNUC__
#pragma GCC diagnostic pop
#endif

TEST_CASE( "./succeeding/exceptions/error messages", "The error messages produced by exceptions caught by Catch matched the expected form" )
{
    Catch::EmbeddedRunner runner;
    using namespace Catch::Matchers;
    
    SECTION( "custom, unexpected", "" )
    {    
        runner.runMatching( "./failing/exceptions/custom" );
//        CHECK_THAT( runner.getLog(), Contains( "Unexpected exception" ) ); // Mock reporter doesn't say this
        CHECK_THAT( runner.getLog(), Contains( "custom exception" ) );
    }
    
    SECTION( "in section", "" )
    {    
        runner.runMatching( "./failing/exceptions/in-section" );
        INFO( runner.getLog() );
//        CHECK( runner.getLog().find( "Unexpected exception" ) != std::string::npos ); // Mock reporter doesn't say this
        CHECK_THAT( runner.getLog(), Contains( "Exception from section" ) );
//        CHECK( runner.getLog().find( CATCH_GET_LINE_INFO( "the section2" ) ) != std::string::npos ); // Mock reporter doesn't say this
    }
    
}

inline int thisFunctionNotImplemented( int ) {
    CATCH_NOT_IMPLEMENTED;
}

TEST_CASE( "./succeeding/exceptions/notimplemented", "" )
{
    REQUIRE_THROWS( thisFunctionNotImplemented( 7 ) );
}
