/*
 *  Created by Phil on 21/02/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#include <sstream>
#include <algorithm>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wpadded"
#endif

namespace { namespace MatchersTests {

#ifndef CATCH_CONFIG_DISABLE_MATCHERS

#ifndef MATCHERS_TEST_HELPERS_INCLUDED // Don't compile this more than once per TU
#define MATCHERS_TEST_HELPERS_INCLUDED

    inline const char *testStringForMatching() {
        return "this string contains 'abc' as a substring";
    }

    inline const char *testStringForMatching2() {
        return "some completely different text that contains one common word";
    }

    inline bool alwaysTrue(int) { return true; }
    inline bool alwaysFalse(int) { return false; }


#ifdef _MSC_VER
#pragma warning(disable:4702) // Unreachable code -- MSVC 19 (VS 2015) sees right through the indirection
#endif

#include <exception>

    struct SpecialException : std::exception {
        SpecialException(int i_) : i(i_) {}

        char const* what() const noexcept override {
            return "SpecialException::what";
        }

        int i;
    };

    struct DerivedException : std::exception {
        char const* what() const noexcept override {
            return "DerivedException::what";
        }
    };

    void doesNotThrow() {}

    [[noreturn]]
    void throwsSpecialException(int i) {
        throw SpecialException{i};
    }

    [[noreturn]]
    void throwsAsInt(int i) {
        throw i;
    }

    [[noreturn]]
    void throwsDerivedException() {
        throw DerivedException{};
    }

    class ExceptionMatcher : public Catch::MatcherBase<SpecialException> {
        int m_expected;
    public:
        ExceptionMatcher(int i) : m_expected(i) {}

        bool match(SpecialException const &se) const override {
            return se.i == m_expected;
        }

        std::string describe() const override {
            std::ostringstream ss;
            ss << "special exception has value of " << m_expected;
            return ss.str();
        }
    };

#endif

    using namespace Catch::Matchers;

#ifdef __DJGPP__
    float nextafter(float from, float to)
    {
        return ::nextafterf(from, to);
    }

    double nextafter(double from, double to)
    {
        return ::nextafter(from, to);
    }
#else
    using std::nextafter;
#endif

    TEST_CASE("String matchers", "[matchers]") {
        REQUIRE_THAT(testStringForMatching(), Contains("string"));
        REQUIRE_THAT(testStringForMatching(), Contains("string", Catch::CaseSensitive::No));
        CHECK_THAT(testStringForMatching(), Contains("abc"));
        CHECK_THAT(testStringForMatching(), Contains("aBC", Catch::CaseSensitive::No));

        CHECK_THAT(testStringForMatching(), StartsWith("this"));
        CHECK_THAT(testStringForMatching(), StartsWith("THIS", Catch::CaseSensitive::No));
        CHECK_THAT(testStringForMatching(), EndsWith("substring"));
        CHECK_THAT(testStringForMatching(), EndsWith(" SuBsTrInG", Catch::CaseSensitive::No));
    }

    TEST_CASE("Contains string matcher", "[.][failing][matchers]") {
        CHECK_THAT(testStringForMatching(), Contains("not there", Catch::CaseSensitive::No));
        CHECK_THAT(testStringForMatching(), Contains("STRING"));
    }

    TEST_CASE("StartsWith string matcher", "[.][failing][matchers]") {
        CHECK_THAT(testStringForMatching(), StartsWith("This String"));
        CHECK_THAT(testStringForMatching(), StartsWith("string", Catch::CaseSensitive::No));
    }

    TEST_CASE("EndsWith string matcher", "[.][failing][matchers]") {
        CHECK_THAT(testStringForMatching(), EndsWith("Substring"));
        CHECK_THAT(testStringForMatching(), EndsWith("this", Catch::CaseSensitive::No));
    }

    TEST_CASE("Equals string matcher", "[.][failing][matchers]") {
        CHECK_THAT(testStringForMatching(), Equals("this string contains 'ABC' as a substring"));
        CHECK_THAT(testStringForMatching(), Equals("something else", Catch::CaseSensitive::No));
    }

    TEST_CASE("Equals", "[matchers]") {
        CHECK_THAT(testStringForMatching(), Equals("this string contains 'abc' as a substring"));
        CHECK_THAT(testStringForMatching(),
                   Equals("this string contains 'ABC' as a substring", Catch::CaseSensitive::No));
    }

// <regex> does not work in libstdc++ 4.8, so we have to enable these tests only when they
// are expected to pass and cannot have them in baselines
    TEST_CASE("Regex string matcher -- libstdc++-4.8 workaround", "[matchers][approvals]") {

// This is fiiiine
// Taken from an answer at
// https://stackoverflow.com/questions/12530406/is-gcc-4-8-or-earlier-buggy-about-regular-expressions
#if (!defined(__GNUC__)) || \
      (__cplusplus >= 201103L && \
      (!defined(__GLIBCXX__) || (__cplusplus >= 201402L) || \
        (defined(_GLIBCXX_REGEX_DFS_QUANTIFIERS_LIMIT) || \
          defined(_GLIBCXX_REGEX_STATE_LIMIT) || \
             (defined(_GLIBCXX_RELEASE) && \
             _GLIBCXX_RELEASE > 4))))

// DJGPP meets the above condition but <regex> does not work properly anyway
#ifndef __DJGPP__
            REQUIRE_THAT(testStringForMatching(), Matches("this string contains 'abc' as a substring"));
            REQUIRE_THAT(testStringForMatching(),
                         Matches("this string CONTAINS 'abc' as a substring", Catch::CaseSensitive::No));
            REQUIRE_THAT(testStringForMatching(), Matches("^this string contains 'abc' as a substring$"));
            REQUIRE_THAT(testStringForMatching(), Matches("^.* 'abc' .*$"));
            REQUIRE_THAT(testStringForMatching(), Matches("^.* 'ABC' .*$", Catch::CaseSensitive::No));
#endif

#endif

            REQUIRE_THAT(testStringForMatching2(), !Matches("this string contains 'abc' as a substring"));
        }

        TEST_CASE("Regex string matcher", "[matchers][.failing]") {
            CHECK_THAT(testStringForMatching(), Matches("this STRING contains 'abc' as a substring"));
            CHECK_THAT(testStringForMatching(), Matches("contains 'abc' as a substring"));
            CHECK_THAT(testStringForMatching(), Matches("this string contains 'abc' as a"));
        }

        TEST_CASE("Matchers can be (AllOf) composed with the && operator", "[matchers][operators][operator&&]") {
            CHECK_THAT(testStringForMatching(),
                       Contains("string") &&
                       Contains("abc") &&
                       Contains("substring") &&
                       Contains("contains"));
        }

        TEST_CASE("Matchers can be (AnyOf) composed with the || operator", "[matchers][operators][operator||]") {
            CHECK_THAT(testStringForMatching(), Contains("string") || Contains("different") || Contains("random"));
            CHECK_THAT(testStringForMatching2(), Contains("string") || Contains("different") || Contains("random"));
        }

        TEST_CASE("Matchers can be composed with both && and ||", "[matchers][operators][operator||][operator&&]") {
            CHECK_THAT(testStringForMatching(), (Contains("string") || Contains("different")) && Contains("substring"));
        }

        TEST_CASE("Matchers can be composed with both && and || - failing",
                  "[matchers][operators][operator||][operator&&][.failing]") {
            CHECK_THAT(testStringForMatching(), (Contains("string") || Contains("different")) && Contains("random"));
        }

        TEST_CASE("Matchers can be negated (Not) with the ! operator", "[matchers][operators][not]") {
            CHECK_THAT(testStringForMatching(), !Contains("different"));
        }

        TEST_CASE("Matchers can be negated (Not) with the ! operator - failing",
                  "[matchers][operators][not][.failing]") {
            CHECK_THAT(testStringForMatching(), !Contains("substring"));
        }

        TEST_CASE("Vector matchers", "[matchers][vector]") {
            std::vector<int> v;
            v.push_back(1);
            v.push_back(2);
            v.push_back(3);

            std::vector<int> v2;
            v2.push_back(1);
            v2.push_back(2);

            std::vector<double> v3;
            v3.push_back(1);
            v3.push_back(2);
            v3.push_back(3);

            std::vector<double> v4;
            v4.push_back(1 + 1e-8);
            v4.push_back(2 + 1e-8);
            v4.push_back(3 + 1e-8);

            std::vector<int> empty;

            SECTION("Contains (element)") {
                CHECK_THAT(v, VectorContains(1));
                CHECK_THAT(v, VectorContains(2));
            }
            SECTION("Contains (vector)") {
                CHECK_THAT(v, Contains(v2));
                v2.push_back(3); // now exactly matches
                CHECK_THAT(v, Contains(v2));

                CHECK_THAT(v, Contains(empty));
                CHECK_THAT(empty, Contains(empty));
            }
            SECTION("Contains (element), composed") {
                CHECK_THAT(v, VectorContains(1) && VectorContains(2));
            }

            SECTION("Equals") {

                // Same vector
                CHECK_THAT(v, Equals(v));

                CHECK_THAT(empty, Equals(empty));

                // Different vector with same elements
                v2.push_back(3);
                CHECK_THAT(v, Equals(v2));
            }
            SECTION("UnorderedEquals") {
                CHECK_THAT(v, UnorderedEquals(v));
                CHECK_THAT(empty, UnorderedEquals(empty));

                auto permuted = v;
                std::next_permutation(begin(permuted), end(permuted));
                REQUIRE_THAT(permuted, UnorderedEquals(v));

                std::reverse(begin(permuted), end(permuted));
                REQUIRE_THAT(permuted, UnorderedEquals(v));
            }
        }

        TEST_CASE("Vector matchers that fail", "[matchers][vector][.][failing]") {
            std::vector<int> v;
            v.push_back(1);
            v.push_back(2);
            v.push_back(3);

            std::vector<int> v2;
            v2.push_back(1);
            v2.push_back(2);

            std::vector<double> v3;
            v3.push_back(1);
            v3.push_back(2);
            v3.push_back(3);

            std::vector<double> v4;
            v4.push_back(1.1);
            v4.push_back(2.1);
            v4.push_back(3.1);

            std::vector<int> empty;

            SECTION("Contains (element)") {
                CHECK_THAT(v, VectorContains(-1));
                CHECK_THAT(empty, VectorContains(1));
            }
            SECTION("Contains (vector)") {
                CHECK_THAT(empty, Contains(v));
                v2.push_back(4);
                CHECK_THAT(v, Contains(v2));
            }

            SECTION("Equals") {

                CHECK_THAT(v, Equals(v2));
                CHECK_THAT(v2, Equals(v));
                CHECK_THAT(empty, Equals(v));
                CHECK_THAT(v, Equals(empty));
            }
            SECTION("UnorderedEquals") {
                CHECK_THAT(v, UnorderedEquals(empty));
                CHECK_THAT(empty, UnorderedEquals(v));

                auto permuted = v;
                std::next_permutation(begin(permuted), end(permuted));
                permuted.pop_back();
                CHECK_THAT(permuted, UnorderedEquals(v));

                std::reverse(begin(permuted), end(permuted));
                CHECK_THAT(permuted, UnorderedEquals(v));
            }
        }

        TEST_CASE("Exception matchers that succeed", "[matchers][exceptions][!throws]") {
            CHECK_THROWS_MATCHES(throwsSpecialException(1), SpecialException, ExceptionMatcher{1});
            REQUIRE_THROWS_MATCHES(throwsSpecialException(2), SpecialException, ExceptionMatcher{2});
        }

        TEST_CASE("Exception matchers that fail", "[matchers][exceptions][!throws][.failing]") {
            SECTION("No exception") {
                CHECK_THROWS_MATCHES(doesNotThrow(), SpecialException, ExceptionMatcher{1});
                REQUIRE_THROWS_MATCHES(doesNotThrow(), SpecialException, ExceptionMatcher{1});
            }
            SECTION("Type mismatch") {
                CHECK_THROWS_MATCHES(throwsAsInt(1), SpecialException, ExceptionMatcher{1});
                REQUIRE_THROWS_MATCHES(throwsAsInt(1), SpecialException, ExceptionMatcher{1});
            }
            SECTION("Contents are wrong") {
                CHECK_THROWS_MATCHES(throwsSpecialException(3), SpecialException, ExceptionMatcher{1});
                REQUIRE_THROWS_MATCHES(throwsSpecialException(4), SpecialException, ExceptionMatcher{1});
            }
        }

        TEST_CASE("Floating point matchers: float", "[matchers][floating-point]") {
            SECTION("Relative") {
                REQUIRE_THAT(10.f,  WithinRel(11.1f, 0.1f));
                REQUIRE_THAT(10.f, !WithinRel(11.2f, 0.1f));
                REQUIRE_THAT( 1.f, !WithinRel(0.f, 0.99f));
                REQUIRE_THAT(-0.f,  WithinRel(0.f));
                SECTION("Some subnormal values") {
                    auto v1 = std::numeric_limits<float>::min();
                    auto v2 = v1;
                    for (int i = 0; i < 5; ++i) {
                        v2 = std::nextafter(v1, 0.f);
                    }
                    REQUIRE_THAT(v1, WithinRel(v2));
                }
            }
            SECTION("Margin") {
                REQUIRE_THAT(1.f, WithinAbs(1.f, 0));
                REQUIRE_THAT(0.f, WithinAbs(1.f, 1));

                REQUIRE_THAT(0.f, !WithinAbs(1.f, 0.99f));
                REQUIRE_THAT(0.f, !WithinAbs(1.f, 0.99f));

                REQUIRE_THAT(0.f, WithinAbs(-0.f, 0));

                REQUIRE_THAT(11.f, !WithinAbs(10.f, 0.5f));
                REQUIRE_THAT(10.f, !WithinAbs(11.f, 0.5f));
                REQUIRE_THAT(-10.f, WithinAbs(-10.f, 0.5f));
                REQUIRE_THAT(-10.f, WithinAbs(-9.6f, 0.5f));
            }
            SECTION("ULPs") {
                REQUIRE_THAT(1.f, WithinULP(1.f, 0));

                REQUIRE_THAT(nextafter(1.f, 2.f), WithinULP(1.f, 1));
                REQUIRE_THAT(0.f, WithinULP(nextafter(0.f, 1.f), 1));
                REQUIRE_THAT(1.f, WithinULP(nextafter(1.f, 0.f), 1));
                REQUIRE_THAT(1.f, !WithinULP(nextafter(1.f, 2.f), 0));

                REQUIRE_THAT(1.f, WithinULP(1.f, 0));
                REQUIRE_THAT(-0.f, WithinULP(0.f, 0));
            }
            SECTION("Composed") {
                REQUIRE_THAT(1.f, WithinAbs(1.f, 0.5) || WithinULP(1.f, 1));
                REQUIRE_THAT(1.f, WithinAbs(2.f, 0.5) || WithinULP(1.f, 0));
                REQUIRE_THAT(0.0001f, WithinAbs(0.f, 0.001f) || WithinRel(0.f, 0.1f));
            }
            SECTION("Constructor validation") {
                REQUIRE_NOTHROW(WithinAbs(1.f, 0.f));
                REQUIRE_THROWS_AS(WithinAbs(1.f, -1.f), std::domain_error);

                REQUIRE_NOTHROW(WithinULP(1.f, 0));
                REQUIRE_THROWS_AS(WithinULP(1.f, static_cast<uint64_t>(-1)), std::domain_error);

                REQUIRE_NOTHROW(WithinRel(1.f, 0.f));
                REQUIRE_THROWS_AS(WithinRel(1.f, -0.2f), std::domain_error);
                REQUIRE_THROWS_AS(WithinRel(1.f, 1.f), std::domain_error);
            }
        }

        TEST_CASE("Floating point matchers: double", "[matchers][floating-point]") {
            SECTION("Relative") {
                REQUIRE_THAT(10., WithinRel(11.1, 0.1));
                REQUIRE_THAT(10., !WithinRel(11.2, 0.1));
                REQUIRE_THAT(1., !WithinRel(0., 0.99));
                REQUIRE_THAT(-0., WithinRel(0.));
                SECTION("Some subnormal values") {
                    auto v1 = std::numeric_limits<double>::min();
                    auto v2 = v1;
                    for (int i = 0; i < 5; ++i) {
                        v2 = std::nextafter(v1, 0);
                    }
                    REQUIRE_THAT(v1, WithinRel(v2));
                }
            }
            SECTION("Margin") {
                REQUIRE_THAT(1., WithinAbs(1., 0));
                REQUIRE_THAT(0., WithinAbs(1., 1));

                REQUIRE_THAT(0., !WithinAbs(1., 0.99));
                REQUIRE_THAT(0., !WithinAbs(1., 0.99));

                REQUIRE_THAT(11., !WithinAbs(10., 0.5));
                REQUIRE_THAT(10., !WithinAbs(11., 0.5));
                REQUIRE_THAT(-10., WithinAbs(-10., 0.5));
                REQUIRE_THAT(-10., WithinAbs(-9.6, 0.5));
            }
            SECTION("ULPs") {
                REQUIRE_THAT(1., WithinULP(1., 0));

                REQUIRE_THAT(nextafter(1., 2.), WithinULP(1., 1));
                REQUIRE_THAT(0.,  WithinULP(nextafter(0., 1.), 1));
                REQUIRE_THAT(1.,  WithinULP(nextafter(1., 0.), 1));
                REQUIRE_THAT(1., !WithinULP(nextafter(1., 2.), 0));

                REQUIRE_THAT(1., WithinULP(1., 0));
                REQUIRE_THAT(-0., WithinULP(0., 0));
            }
            SECTION("Composed") {
                REQUIRE_THAT(1., WithinAbs(1., 0.5) || WithinULP(2., 1));
                REQUIRE_THAT(1., WithinAbs(2., 0.5) || WithinULP(1., 0));
                REQUIRE_THAT(0.0001, WithinAbs(0., 0.001) || WithinRel(0., 0.1));
            }
            SECTION("Constructor validation") {
                REQUIRE_NOTHROW(WithinAbs(1., 0.));
                REQUIRE_THROWS_AS(WithinAbs(1., -1.), std::domain_error);

                REQUIRE_NOTHROW(WithinULP(1., 0));

                REQUIRE_NOTHROW(WithinRel(1., 0.));
                REQUIRE_THROWS_AS(WithinRel(1., -0.2), std::domain_error);
                REQUIRE_THROWS_AS(WithinRel(1., 1.), std::domain_error);
            }
        }

        TEST_CASE("Floating point matchers that are problematic in approvals", "[approvals][matchers][floating-point]") {
            REQUIRE_THAT(NAN, !WithinAbs(NAN, 0));
            REQUIRE_THAT(NAN, !(WithinAbs(NAN, 100) || WithinULP(NAN, 123)));
            REQUIRE_THAT(NAN, !WithinULP(NAN, 123));
            REQUIRE_THAT(INFINITY, WithinRel(INFINITY));
            REQUIRE_THAT(-INFINITY, !WithinRel(INFINITY));
            REQUIRE_THAT(1., !WithinRel(INFINITY));
            REQUIRE_THAT(INFINITY, !WithinRel(1.));
            REQUIRE_THAT(NAN, !WithinRel(NAN));
            REQUIRE_THAT(1., !WithinRel(NAN));
            REQUIRE_THAT(NAN, !WithinRel(1.));
        }

        TEST_CASE("Arbitrary predicate matcher", "[matchers][generic]") {
            SECTION("Function pointer") {
                REQUIRE_THAT(1,  Predicate<int>(alwaysTrue, "always true"));
                REQUIRE_THAT(1, !Predicate<int>(alwaysFalse, "always false"));
            }
            SECTION("Lambdas + different type") {
                REQUIRE_THAT("Hello olleH",
                             Predicate<std::string>(
                                 [] (std::string const& str) -> bool { return str.front() == str.back(); },
                                 "First and last character should be equal")
                );

                REQUIRE_THAT("This wouldn't pass",
                             !Predicate<std::string>(
                                 [] (std::string const& str) -> bool { return str.front() == str.back(); }
                             )
                );
            }
        }

        TEST_CASE("Regression test #1", "[matchers][vector]") {
            // At some point, UnorderedEqualsMatcher skipped
            // mismatched prefixed before doing the comparison itself
            std::vector<char> actual = { 'a', 'b' };
            std::vector<char> expected = { 'c', 'b' };

            CHECK_THAT(actual, !UnorderedEquals(expected));
        }

        TEST_CASE("Predicate matcher can accept const char*", "[matchers][compilation]") {
            REQUIRE_THAT("foo", Predicate<const char*>([] (const char* const&) { return true; }));
        }

        TEST_CASE("Vector Approx matcher", "[matchers][approx][vector]") {
            using Catch::Matchers::Approx;
            SECTION("Empty vector is roughly equal to an empty vector") {
                std::vector<double> empty;
                REQUIRE_THAT(empty, Approx(empty));
            }
            SECTION("Vectors with elements") {
                std::vector<double> v1({1., 2., 3.});
                SECTION("A vector is approx equal to itself") {
                    REQUIRE_THAT(v1, Approx(v1));
                }
                std::vector<double> v2({1.5, 2.5, 3.5});
                SECTION("Different length") {
                    auto temp(v1);
                    temp.push_back(4);
                    REQUIRE_THAT(v1, !Approx(temp));
                }
                SECTION("Same length, different elements") {
                    REQUIRE_THAT(v1, !Approx(v2));
                    REQUIRE_THAT(v1, Approx(v2).margin(0.5));
                    REQUIRE_THAT(v1, Approx(v2).epsilon(0.5));
                    REQUIRE_THAT(v1, Approx(v2).epsilon(0.1).scale(500));
                }
            }
        }

        TEST_CASE("Vector Approx matcher -- failing", "[matchers][approx][vector][.failing]") {
            using Catch::Matchers::Approx;
            SECTION("Empty and non empty vectors are not approx equal") {
                std::vector<double> empty, t1({1, 2});
                CHECK_THAT(empty, Approx(t1));
            }
            SECTION("Just different vectors") {
                std::vector<double> v1({2., 4., 6.}), v2({1., 3., 5.});
                CHECK_THAT(v1, Approx(v2));
            }
        }

        TEST_CASE("Exceptions matchers", "[matchers][exceptions][!throws]") {
            REQUIRE_THROWS_MATCHES(throwsDerivedException(),  DerivedException,  Message("DerivedException::what"));
            REQUIRE_THROWS_MATCHES(throwsDerivedException(),  DerivedException, !Message("derivedexception::what"));
            REQUIRE_THROWS_MATCHES(throwsSpecialException(2), SpecialException, !Message("DerivedException::what"));
            REQUIRE_THROWS_MATCHES(throwsSpecialException(2), SpecialException,  Message("SpecialException::what"));
        }
        
        TEST_CASE("Composed matchers are distinct", "[matchers][composed]") {
            auto m1 = Contains("string");
            auto m2 = Contains("random");
            auto composed1 = m1 || m2;
            auto m3 = Contains("different");
            auto composed2 = composed1 || m3;
            REQUIRE_THAT(testStringForMatching2(), !composed1);
            REQUIRE_THAT(testStringForMatching2(), composed2);
        }

} } // namespace MatchersTests

#endif // CATCH_CONFIG_DISABLE_MATCHERS

#ifdef __clang__
#pragma clang diagnostic pop
#endif
