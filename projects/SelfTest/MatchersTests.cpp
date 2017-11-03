/*
 *  Created by Phil on 21/02/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wpadded"
#endif

#ifndef CATCH_CONFIG_DISABLE_MATCHERS

inline const char* testStringForMatching()
{
    return "this string contains 'abc' as a substring";
}
inline const char* testStringForMatching2()
{
    return "some completely different text that contains one common word";
}

using namespace Catch::Matchers;

TEST_CASE("String matchers", "[matchers]" )
{
    REQUIRE_THAT( testStringForMatching(), Contains( "string" ) );
    CHECK_THAT( testStringForMatching(), Contains( "abc" ) );

    CHECK_THAT( testStringForMatching(), StartsWith( "this" ) );
    CHECK_THAT( testStringForMatching(), EndsWith( "substring" ) );
}

TEST_CASE("Contains string matcher", "[.][failing][matchers]")
{
    CHECK_THAT( testStringForMatching(), Contains( "not there" ) );
}

TEST_CASE("StartsWith string matcher", "[.][failing][matchers]")
{
    CHECK_THAT( testStringForMatching(), StartsWith( "string" ) );
}

TEST_CASE("EndsWith string matcher", "[.][failing][matchers]")
{
    CHECK_THAT( testStringForMatching(), EndsWith( "this" ) );
}

TEST_CASE("Equals string matcher", "[.][failing][matchers]")
{
    CHECK_THAT( testStringForMatching(), Equals( "something else" ) );
}

TEST_CASE("Equals", "[matchers]")
{
    CHECK_THAT( testStringForMatching(), Equals( "this string contains 'abc' as a substring" ) );
}

TEST_CASE("Matchers can be (AllOf) composed with the && operator", "[matchers][operators][operator&&]")
{
    CHECK_THAT( testStringForMatching(),
                Contains( "string" ) &&
                Contains( "abc" ) &&
                Contains( "substring" ) &&
                Contains( "contains" ) );
}

TEST_CASE("Matchers can be (AnyOf) composed with the || operator", "[matchers][operators][operator||]")
{
    CHECK_THAT( testStringForMatching(), Contains( "string" ) || Contains( "different" ) || Contains( "random" ) );
    CHECK_THAT( testStringForMatching2(), Contains( "string" ) || Contains( "different" ) || Contains( "random" ) );
}

TEST_CASE("Matchers can be composed with both && and ||", "[matchers][operators][operator||][operator&&]")
{
    CHECK_THAT( testStringForMatching(), ( Contains( "string" ) || Contains( "different" ) ) && Contains( "substring" ) );
}

TEST_CASE("Matchers can be composed with both && and || - failing", "[matchers][operators][operator||][operator&&][.failing]")
{
    CHECK_THAT( testStringForMatching(), ( Contains( "string" ) || Contains( "different" ) ) && Contains( "random" ) );
}

TEST_CASE("Matchers can be negated (Not) with the ! operator", "[matchers][operators][not]")
{
    CHECK_THAT( testStringForMatching(), !Contains( "different" ) );
}

TEST_CASE("Matchers can be negated (Not) with the ! operator - failing", "[matchers][operators][not][.failing]")
{
    CHECK_THAT( testStringForMatching(), !Contains( "substring" ) );
}

TEST_CASE( "Vector matchers", "[matchers][vector]" ) {
    std::vector<int> v;
    v.push_back( 1 );
    v.push_back( 2 );
    v.push_back( 3 );

    std::vector<int> v2;
    v2.push_back( 1 );
    v2.push_back( 2 );

    std::vector<int> empty;

    SECTION( "Contains (element)" ) {
        CHECK_THAT( v, VectorContains( 1 ) );
        CHECK_THAT( v, VectorContains( 2 ) );
    }
    SECTION( "Contains (vector)" ) {
        CHECK_THAT( v, Contains( v2 ) );
        v2.push_back( 3 ); // now exactly matches
        CHECK_THAT( v, Contains( v2 ) );

        CHECK_THAT( v, Contains( empty) );
        CHECK_THAT( empty, Contains( empty) );
    }

    SECTION( "Equals" ) {

        // Same vector
        CHECK_THAT( v, Equals( v ) );

        CHECK_THAT( empty, Equals( empty ) );

        // Different vector with same elements
        v2.push_back( 3 );
        CHECK_THAT( v, Equals( v2 ) );
    }
}

TEST_CASE( "Vector matchers that fail", "[matchers][vector][.][failing]" ) {
    std::vector<int> v;
    v.push_back( 1 );
    v.push_back( 2 );
    v.push_back( 3 );

    std::vector<int> v2;
    v2.push_back( 1 );
    v2.push_back( 2 );

    std::vector<int> empty;

    SECTION( "Contains (element)" ) {
        CHECK_THAT( v, VectorContains( -1 ) );
        CHECK_THAT( empty, VectorContains( 1 ) );
    }
    SECTION( "Contains (vector)" ) {
        CHECK_THAT( empty, Contains( v) );
        v2.push_back( 4 );
        CHECK_THAT( v, Contains( v2 ) );
    }

    SECTION( "Equals" ) {

        CHECK_THAT( v, Equals( v2 ) );
        CHECK_THAT( v2, Equals( v ) );
        CHECK_THAT( empty, Equals( v ) );
        CHECK_THAT( v, Equals( empty ) );
    }
}

#ifdef _MSC_VER
#pragma warning(disable:4702) // Unreachable code -- MSVC 19 (VS 2015) sees right through the indirection
#endif

#include <exception>

struct SpecialException : std::exception {
    SpecialException(int i_):i(i_) {}
    int i;
};

void doesNotThrow() {}

[[noreturn]]
void throws(int i) {
    throw SpecialException{ i };
}

[[noreturn]]
void throwsAsInt(int i) {
    throw i;
}

class ExceptionMatcher : public Catch::MatcherBase<SpecialException> {
    int m_expected;
public:
    ExceptionMatcher(int i):m_expected(i) {}
    bool match(SpecialException const& se) const override {
        return se.i == m_expected;
    }

    std::string describe() const override {
        std::ostringstream ss;
        ss << "special exception has value of " << m_expected;
        return ss.str();
    }
};


TEST_CASE( "Exception matchers that succeed", "[matchers][exceptions][!throws]" ) {
    CHECK_THROWS_MATCHES(throws(1), SpecialException, ExceptionMatcher{ 1 });
    REQUIRE_THROWS_MATCHES(throws(2), SpecialException, ExceptionMatcher{ 2 });
}

TEST_CASE("Exception matchers that fail", "[matchers][exceptions][!throws][.failing]") {
    SECTION("No exception") {
        CHECK_THROWS_MATCHES(doesNotThrow(), SpecialException, ExceptionMatcher{ 1 });
        REQUIRE_THROWS_MATCHES(doesNotThrow(), SpecialException, ExceptionMatcher{ 1 });
    }
    SECTION("Type mismatch") {
        CHECK_THROWS_MATCHES(throwsAsInt(1), SpecialException, ExceptionMatcher{ 1 });
        REQUIRE_THROWS_MATCHES(throwsAsInt(1), SpecialException, ExceptionMatcher{ 1 });
    }
    SECTION("Contents are wrong") {
        CHECK_THROWS_MATCHES(throws(3), SpecialException, ExceptionMatcher{ 1 });
        REQUIRE_THROWS_MATCHES(throws(4), SpecialException, ExceptionMatcher{ 1 });
    }
}

#endif // CATCH_CONFIG_DISABLE_MATCHERS

#ifdef __clang__
#pragma clang diagnostic pop
#endif
