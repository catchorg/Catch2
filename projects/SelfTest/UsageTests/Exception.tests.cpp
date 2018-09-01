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

#ifdef _MSC_VER
#pragma warning(disable:4702) // Unreachable code -- unconditional throws and so on
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wunreachable-code"
#endif

namespace { namespace ExceptionTests {

#ifndef EXCEPTION_TEST_HELPERS_INCLUDED // Don't compile this more than once per TU
#define EXCEPTION_TEST_HELPERS_INCLUDED

int thisThrows() {
    throw std::domain_error( "expected exception" );
    return 1;
}

int thisDoesntThrow() {
    return 0;
}

class CustomException {
public:
    explicit CustomException( const std::string& msg )
    : m_msg( msg )
    {}

    std::string getMessage() const {
        return m_msg;
    }

private:
    std::string m_msg;
};

class CustomStdException : public std::exception {
public:
    explicit CustomStdException( const std::string& msg )
    : m_msg( msg )
    {}
    ~CustomStdException() noexcept override {}

    std::string getMessage() const {
        return m_msg;
    }

private:
    std::string m_msg;
};

[[noreturn]] void throwCustom() {
    throw CustomException( "custom exception - not std" );
}

#endif

TEST_CASE( "When checked exceptions are thrown they can be expected or unexpected", "[!throws]" ) {
    REQUIRE_THROWS_AS( thisThrows(), std::domain_error );
    REQUIRE_NOTHROW( thisDoesntThrow() );
    REQUIRE_THROWS( thisThrows() );
}

TEST_CASE( "Expected exceptions that don't throw or unexpected exceptions fail the test", "[.][failing][!throws]" ) {
    CHECK_THROWS_AS( thisThrows(), std::string );
    CHECK_THROWS_AS( thisDoesntThrow(), std::domain_error );
    CHECK_NOTHROW( thisThrows() );
}

TEST_CASE( "When unchecked exceptions are thrown directly they are always failures", "[.][failing][!throws]" ) {
    throw std::domain_error( "unexpected exception" );
}

TEST_CASE( "An unchecked exception reports the line of the last assertion", "[.][failing][!throws]" ) {
    CHECK( 1 == 1 );
    throw std::domain_error( "unexpected exception" );
}

TEST_CASE( "When unchecked exceptions are thrown from sections they are always failures", "[.][failing][!throws]" ) {
    SECTION( "section name" ) {
        throw std::domain_error("unexpected exception");
    }
}

TEST_CASE( "When unchecked exceptions are thrown from functions they are always failures", "[.][failing][!throws]" ) {
    CHECK( thisThrows() == 0 );
}

TEST_CASE( "When unchecked exceptions are thrown during a REQUIRE the test should abort fail", "[.][failing][!throws]" ) {
    REQUIRE( thisThrows() == 0 );
    FAIL( "This should never happen" );
}

TEST_CASE( "When unchecked exceptions are thrown during a CHECK the test should continue", "[.][failing][!throws]" ) {
    try {
        CHECK(thisThrows() == 0);
    }
    catch(...) {
        FAIL( "This should never happen" );
    }
}

TEST_CASE( "When unchecked exceptions are thrown, but caught, they do not affect the test", "[!throws]" ) {
    try {
        throw std::domain_error( "unexpected exception" );
    }
    catch(...) {}
}


CATCH_TRANSLATE_EXCEPTION( CustomException& ex ) {
    return ex.getMessage();
}

CATCH_TRANSLATE_EXCEPTION( CustomStdException& ex ) {
    return ex.getMessage();
}

CATCH_TRANSLATE_EXCEPTION( double& ex ) {
    return Catch::Detail::stringify( ex );
}

TEST_CASE("Non-std exceptions can be translated", "[.][failing][!throws]" ) {
    throw CustomException( "custom exception" );
}

TEST_CASE("Custom std-exceptions can be custom translated", "[.][failing][!throws]" ) {
    throw CustomException( "custom std exception" );
}

TEST_CASE( "Custom exceptions can be translated when testing for nothrow", "[.][failing][!throws]" ) {
    REQUIRE_NOTHROW( throwCustom() );
}

TEST_CASE( "Custom exceptions can be translated when testing for throwing as something else", "[.][failing][!throws]" ) {
    REQUIRE_THROWS_AS( throwCustom(), std::exception );
}

TEST_CASE( "Unexpected exceptions can be translated", "[.][failing][!throws]"  ) {
    throw double( 3.14 );
}

TEST_CASE("Thrown string literals are translated", "[.][failing][!throws]") {
    throw "For some reason someone is throwing a string literal!";
}

TEST_CASE("thrown std::strings are translated", "[.][failing][!throws]") {
    throw std::string{ "Why would you throw a std::string?" };
}


#ifndef CATCH_CONFIG_DISABLE_MATCHERS

TEST_CASE( "Exception messages can be tested for", "[!throws]" ) {
    using namespace Catch::Matchers;
    SECTION( "exact match" )
        REQUIRE_THROWS_WITH( thisThrows(), "expected exception" );
    SECTION( "different case" )
    REQUIRE_THROWS_WITH( thisThrows(), Equals( "expecteD Exception", Catch::CaseSensitive::No ) );
    SECTION( "wildcarded" ) {
        REQUIRE_THROWS_WITH( thisThrows(), StartsWith( "expected" ) );
        REQUIRE_THROWS_WITH( thisThrows(), EndsWith( "exception" ) );
        REQUIRE_THROWS_WITH( thisThrows(), Contains( "except" ) );
        REQUIRE_THROWS_WITH( thisThrows(), Contains( "exCept", Catch::CaseSensitive::No ) );
    }
}

#endif

TEST_CASE( "Mismatching exception messages failing the test", "[.][failing][!throws]" ) {
    REQUIRE_THROWS_WITH( thisThrows(), "expected exception" );
    REQUIRE_THROWS_WITH( thisThrows(), "should fail" );
    REQUIRE_THROWS_WITH( thisThrows(), "expected exception" );
}

TEST_CASE( "#748 - captures with unexpected exceptions", "[.][failing][!throws][!shouldfail]" ) {
    int answer = 42;
    CAPTURE( answer );
    // the message should be printed on the first two sections but not on the third
    SECTION( "outside assertions" ) {
        thisThrows();
    }
    SECTION( "inside REQUIRE_NOTHROW" ) {
        REQUIRE_NOTHROW( thisThrows() );
    }
    SECTION( "inside REQUIRE_THROWS" ) {
        REQUIRE_THROWS( thisThrows() );
    }
}

}} // namespace ExceptionTests

#ifdef __clang__
#pragma clang diagnostic pop
#endif
