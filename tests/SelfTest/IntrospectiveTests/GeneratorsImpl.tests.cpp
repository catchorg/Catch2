
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#if defined( __GNUC__ ) || defined( __clang__ )
#    pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <helpers/range_test_helpers.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generator_exception.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/generators/catch_generators_range.hpp>

// Tests of generator implementation details
TEST_CASE("Generators internals", "[generators][internals]") {
    using namespace Catch::Generators;

    SECTION("Single value") {
        auto gen = value(123);
        REQUIRE(gen.get() == 123);
        REQUIRE_FALSE(gen.next());
    }
    SECTION("Preset values") {
        auto gen = values({ 1, 3, 5 });
        REQUIRE(gen.get() == 1);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 3);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 5);
        REQUIRE_FALSE(gen.next());
    }
    SECTION("Generator combinator") {
        auto gen = makeGenerators(1, 5, values({ 2, 4 }), 0);
        REQUIRE(gen.get() == 1);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 5);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 2);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 4);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 0);
        REQUIRE_FALSE(gen.next());
    }
    SECTION("Explicitly typed generator sequence") {
        auto gen = makeGenerators(as<std::string>{}, "aa", "bb", "cc");
        // This just checks that the type is std::string:
        REQUIRE(gen.get().size() == 2);
        // Iterate over the generator
        REQUIRE(gen.get() == "aa");
        REQUIRE(gen.next());
        REQUIRE(gen.get() == "bb");
        REQUIRE(gen.next());
        REQUIRE(gen.get() == "cc");
        REQUIRE_FALSE(gen.next());
    }
    SECTION("Filter generator") {
        // Normal usage
        SECTION("Simple filtering") {
            auto gen = filter([](int i) { return i != 2; }, values({ 2, 1, 2, 3, 2, 2 }));
            REQUIRE(gen.get() == 1);
            REQUIRE(gen.next());
            REQUIRE(gen.get() == 3);
            REQUIRE_FALSE(gen.next());
        }
        SECTION("Filter out multiple elements at the start and end") {
            auto gen = filter([](int i) { return i != 2; }, values({ 2, 2, 1, 3, 2, 2 }));
            REQUIRE(gen.get() == 1);
            REQUIRE(gen.next());
            REQUIRE(gen.get() == 3);
            REQUIRE_FALSE(gen.next());
        }

        SECTION("Throws on construction if it can't get initial element") {
            REQUIRE_THROWS_AS(filter([](int) { return false; }, value(1)), Catch::GeneratorException);
            REQUIRE_THROWS_AS(
                filter([](int) { return false; }, values({ 1, 2, 3 })),
                Catch::GeneratorException);
        }

        // Non-trivial usage
        SECTION("Out-of-line predicates are copied into the generator") {
            auto evilNumber = Catch::Detail::make_unique<int>(2);
            auto gen = [&]{
                const auto predicate = [&](int i) { return i != *evilNumber; };
                return filter(predicate, values({ 2, 1, 2, 3, 2, 2 }));
            }();
            REQUIRE(gen.get() == 1);
            REQUIRE(gen.next());
            REQUIRE(gen.get() == 3);
            REQUIRE_FALSE(gen.next());
        }
    }
    SECTION("Take generator") {
        SECTION("Take less") {
            auto gen = take(2, values({ 1, 2, 3 }));
            REQUIRE(gen.get() == 1);
            REQUIRE(gen.next());
            REQUIRE(gen.get() == 2);
            REQUIRE_FALSE(gen.next());
        }
        SECTION("Take more") {
            auto gen = take(2, value(1));
            REQUIRE(gen.get() == 1);
            REQUIRE_FALSE(gen.next());
        }
    }
    SECTION("Map with explicit return type") {
        auto gen = map<double>([] (int i) {return 2.0 * i; }, values({ 1, 2, 3 }));
        REQUIRE(gen.get() == 2.0);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 4.0);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 6.0);
        REQUIRE_FALSE(gen.next());
    }
    SECTION("Map with deduced return type") {
        auto gen = map([] (int i) {return 2.0 * i; }, values({ 1, 2, 3 }));
        REQUIRE(gen.get() == 2.0);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 4.0);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 6.0);
        REQUIRE_FALSE(gen.next());
    }
    SECTION("Repeat") {
        SECTION("Singular repeat") {
            auto gen = repeat(1, value(3));
            REQUIRE(gen.get() == 3);
            REQUIRE_FALSE(gen.next());
        }
        SECTION("Actual repeat") {
            auto gen = repeat(2, values({ 1, 2, 3 }));
            REQUIRE(gen.get() == 1);
            REQUIRE(gen.next());
            REQUIRE(gen.get() == 2);
            REQUIRE(gen.next());
            REQUIRE(gen.get() == 3);
            REQUIRE(gen.next());
            REQUIRE(gen.get() == 1);
            REQUIRE(gen.next());
            REQUIRE(gen.get() == 2);
            REQUIRE(gen.next());
            REQUIRE(gen.get() == 3);
            REQUIRE_FALSE(gen.next());
        }
    }
    SECTION("Range") {
        SECTION("Positive auto step") {
            SECTION("Integer") {
                auto gen = range(-2, 2);
                REQUIRE(gen.get() == -2);
                REQUIRE(gen.next());
                REQUIRE(gen.get() == -1);
                REQUIRE(gen.next());
                REQUIRE(gen.get() == 0);
                REQUIRE(gen.next());
                REQUIRE(gen.get() == 1);
                REQUIRE_FALSE(gen.next());
            }
        }
        SECTION("Negative auto step") {
            SECTION("Integer") {
                auto gen = range(2, -2);
                REQUIRE(gen.get() == 2);
                REQUIRE(gen.next());
                REQUIRE(gen.get() == 1);
                REQUIRE(gen.next());
                REQUIRE(gen.get() == 0);
                REQUIRE(gen.next());
                REQUIRE(gen.get() == -1);
                REQUIRE_FALSE(gen.next());
            }
        }
        SECTION("Positive manual step") {
            SECTION("Integer") {
                SECTION("Exact") {
                    auto gen = range(-7, 5, 3);
                    REQUIRE(gen.get() == -7);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -4);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -1);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == 2);
                    REQUIRE_FALSE(gen.next());
                }
                SECTION("Slightly over end") {
                    auto gen = range(-7, 4, 3);
                    REQUIRE(gen.get() == -7);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -4);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -1);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == 2);
                    REQUIRE_FALSE(gen.next());
                }
                SECTION("Slightly under end") {
                    auto gen = range(-7, 6, 3);
                    REQUIRE(gen.get() == -7);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -4);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -1);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == 2);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == 5);
                    REQUIRE_FALSE(gen.next());
                }
            }

            SECTION("Floating Point") {
                using Catch::Approx;
                SECTION("Exact") {
                    const auto rangeStart = -1.;
                    const auto rangeEnd = 1.;
                    const auto step = .1;

                    auto gen = range(rangeStart, rangeEnd, step);
                    auto expected = rangeStart;
                    while( (rangeEnd - expected) > step ) {
                        INFO( "Current expected value is " << expected );
                        REQUIRE(gen.get() == Approx(expected));
                        REQUIRE(gen.next());

                        expected += step;
                    }
                    REQUIRE(gen.get() == Approx( rangeEnd ) );
                    REQUIRE_FALSE(gen.next());
                }
                SECTION("Slightly over end") {
                    const auto rangeStart = -1.;
                    const auto rangeEnd = 1.;
                    const auto step = .3;

                    auto gen = range(rangeStart, rangeEnd, step);
                    auto expected = rangeStart;
                    while( (rangeEnd - expected) > step ) {
                       INFO( "Current expected value is " << expected );
                       REQUIRE(gen.get() == Approx(expected));
                       REQUIRE(gen.next());

                       expected += step;
                    }
                    REQUIRE_FALSE(gen.next());
                }
                SECTION("Slightly under end") {
                    const auto rangeStart = -1.;
                    const auto rangeEnd = .9;
                    const auto step = .3;

                    auto gen = range(rangeStart, rangeEnd, step);
                    auto expected = rangeStart;
                    while( (rangeEnd - expected) > step ) {
                       INFO( "Current expected value is " << expected );
                       REQUIRE(gen.get() == Approx(expected));
                       REQUIRE(gen.next());

                       expected += step;
                    }
                    REQUIRE_FALSE(gen.next());
                }
            }
        }
        SECTION("Negative manual step") {
            SECTION("Integer") {
                SECTION("Exact") {
                    auto gen = range(5, -7, -3);
                    REQUIRE(gen.get() == 5);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == 2);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -1);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -4);
                    REQUIRE_FALSE(gen.next());
                }
                SECTION("Slightly over end") {
                    auto gen = range(5, -6, -3);
                    REQUIRE(gen.get() == 5);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == 2);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -1);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -4);
                    REQUIRE_FALSE(gen.next());
                }
                SECTION("Slightly under end") {
                    auto gen = range(5, -8, -3);
                    REQUIRE(gen.get() == 5);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == 2);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -1);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -4);
                    REQUIRE(gen.next());
                    REQUIRE(gen.get() == -7);
                    REQUIRE_FALSE(gen.next());
                }
            }
        }
    }

}


// todo: uncopyable type used in a generator
//  idea: uncopyable tag type for a stupid generator

namespace {
struct non_copyable {
    non_copyable() = default;
    non_copyable(non_copyable const&) = delete;
    non_copyable& operator=(non_copyable const&) = delete;
    int value = -1;
};

// This class shows how to implement a simple generator for Catch tests
class TestGen : public Catch::Generators::IGenerator<int> {
    int current_number;
public:

    TestGen(non_copyable const& nc):
        current_number(nc.value) {}

    int const& get() const override;
    bool next() override {
        return false;
    }
    bool isFinite() const override { return true; }
};

// Avoids -Wweak-vtables
int const& TestGen::get() const {
    return current_number;
}

}

TEST_CASE("GENERATE capture macros", "[generators][internals][approvals]") {
    auto value = GENERATE(take(10, random(0, 10)));

    non_copyable nc; nc.value = value;
    // neither `GENERATE_COPY` nor plain `GENERATE` would compile here
    auto value2 = GENERATE_REF(Catch::Generators::GeneratorWrapper<int>(Catch::Detail::make_unique<TestGen>(nc)));
    REQUIRE(value == value2);
}

TEST_CASE("#1809 - GENERATE_COPY and SingleValueGenerator does not compile", "[generators][compilation][approvals]") {
    // Verify Issue #1809 fix, only needs to compile.
    auto a = GENERATE_COPY(1, 2);
    (void)a;
    auto b = GENERATE_COPY(as<long>{}, 1, 2);
    (void)b;
    int i = 1;
    int j = 2;
    auto c = GENERATE_COPY(i, j);
    (void)c;
    auto d = GENERATE_COPY(as<long>{}, i, j);
    (void)d;
    SUCCEED();
}

TEST_CASE("Multiple random generators in one test case output different values", "[generators][internals][approvals]") {
    SECTION("Integer") {
        auto random1 = Catch::Generators::random(0, 1000);
        auto random2 = Catch::Generators::random(0, 1000);
        size_t same = 0;
        for (size_t i = 0; i < 1000; ++i) {
            same += random1.get() == random2.get();
            random1.next(); random2.next();
        }
        // Because the previous low bound failed CI couple of times,
        // we use a very high threshold of 20% before failure is reported.
        REQUIRE(same < 200);
    }
    SECTION("double") {
        auto random1 = Catch::Generators::random(0., 1000.);
        auto random2 = Catch::Generators::random(0., 1000.);
        size_t same = 0;
        for (size_t i = 0; i < 1000; ++i) {
            same += random1.get() == random2.get();
            random1.next(); random2.next();
        }
        // Because the previous low bound failed CI couple of times,
        // we use a very high threshold of 20% before failure is reported.
        REQUIRE(same < 200);
    }
}

TEST_CASE("#2040 - infinite compilation recursion in GENERATE with MSVC", "[generators][compilation][approvals]") {
    int x = 42;
    auto test = GENERATE_COPY(1, x, 2 * x);
    CHECK(test < 100);
}

namespace {
    static bool always_true(int) {
        return true;
    }

    static bool is_even(int n) {
        return n % 2 == 0;
    }

    static bool is_multiple_of_3(int n) {
        return n % 3 == 0;
    }
}

TEST_CASE("GENERATE handles function (pointers)", "[generators][compilation][approvals]") {
    auto f = GENERATE(always_true, is_even, is_multiple_of_3);
    REQUIRE(f(6));
}

TEST_CASE("GENERATE decays arrays", "[generators][compilation][approvals]") {
    auto str = GENERATE("abc", "def", "gh");
    (void)str;
    STATIC_REQUIRE(std::is_same<decltype(str), const char*>::value);
}

TEST_CASE("Generators count returned elements", "[generators][approvals]") {
    auto generator = Catch::Generators::FixedValuesGenerator<int>( { 1, 2, 3 } );
    REQUIRE( generator.currentElementIndex() == 0 );
    REQUIRE( generator.countedNext() );
    REQUIRE( generator.currentElementIndex() == 1 );
    REQUIRE( generator.countedNext() );
    REQUIRE( generator.currentElementIndex() == 2 );
    REQUIRE_FALSE( generator.countedNext() );
    REQUIRE( generator.currentElementIndex() == 2 );
}

TEST_CASE( "Generators can stringify their elements",
           "[generators][approvals]" ) {
    auto generator =
        Catch::Generators::FixedValuesGenerator<int>( { 1, 2, 3 } );

    REQUIRE( generator.currentElementAsString() == "1"_catch_sr );
    REQUIRE( generator.countedNext() );
    REQUIRE( generator.currentElementAsString() == "2"_catch_sr );
    REQUIRE( generator.countedNext() );
    REQUIRE( generator.currentElementAsString() == "3"_catch_sr );
}

namespace {
    class CustomStringifyGenerator
        : public Catch::Generators::IGenerator<bool> {
        bool m_first = true;

        std::string stringifyImpl() const override {
            return m_first ? "first" : "second";
        }

        bool next() override {
            if ( m_first ) {
                m_first = false;
                return true;
            }
            return false;
        }

    public:
        bool const& get() const override;
        bool isFinite() const override { return true; }
    };

    // Avoids -Wweak-vtables
    bool const& CustomStringifyGenerator::get() const { return m_first; }
} // namespace

TEST_CASE( "Generators can override element stringification",
           "[generators][approvals]" ) {
    CustomStringifyGenerator generator;
    REQUIRE( generator.currentElementAsString() == "first"_catch_sr );
    REQUIRE( generator.countedNext() );
    REQUIRE( generator.currentElementAsString() == "second"_catch_sr );
}

namespace {
    class StringifyCountingGenerator
        : public Catch::Generators::IGenerator<bool> {
        bool m_first = true;
        mutable size_t m_stringificationCalls = 0;

        std::string stringifyImpl() const override {
            ++m_stringificationCalls;
            return m_first ? "first" : "second";
        }

        bool next() override {
            if ( m_first ) {
                m_first = false;
                return true;
            }
            return false;
        }

    public:

        bool const& get() const override;
        size_t stringificationCalls() const { return m_stringificationCalls; }
        bool isFinite() const override { return true; }
    };

    // Avoids -Wweak-vtables
    bool const& StringifyCountingGenerator::get() const { return m_first; }

} // namespace

TEST_CASE( "Generator element stringification is cached",
           "[generators][approvals]" ) {
    StringifyCountingGenerator generator;
    REQUIRE( generator.currentElementAsString() == "first"_catch_sr );
    REQUIRE( generator.currentElementAsString() == "first"_catch_sr );
    REQUIRE( generator.currentElementAsString() == "first"_catch_sr );
    REQUIRE( generator.currentElementAsString() == "first"_catch_sr );
    REQUIRE( generator.currentElementAsString() == "first"_catch_sr );

    REQUIRE( generator.stringificationCalls() == 1 );
}

TEST_CASE( "Random generators can be seeded", "[generators][approvals]" ) {
    SECTION( "Integer generator" ) {
        using Catch::Generators::RandomIntegerGenerator;
        RandomIntegerGenerator<int> rng1( 0, 100, 0x1234 ),
                                    rng2( 0, 100, 0x1234 );

        for ( size_t i = 0; i < 10; ++i ) {
            REQUIRE( rng1.get() == rng2.get() );
            rng1.next(); rng2.next();
        }
    }
    SECTION("Double generator") {
        using Catch::Generators::RandomFloatingGenerator;
        RandomFloatingGenerator<double> rng1( 0., 100., 0x1234 ),
                                        rng2( 0., 100., 0x1234 );
        for ( size_t i = 0; i < 10; ++i ) {
            REQUIRE( rng1.get() == rng2.get() );
            rng1.next();
            rng2.next();
        }
    }
}

TEST_CASE("Filter generator throws exception for empty generator",
          "[generators]") {
    using namespace Catch::Generators;

    REQUIRE_THROWS_AS(
        filter( []( int ) { return false; }, value( 3 ) ),
        Catch::GeneratorException );
}

TEST_CASE("from_range(container) supports ADL begin/end and arrays", "[generators][from-range][approvals]") {
    using namespace Catch::Generators;

    SECTION("C array") {
        int arr[3]{ 5, 6, 7 };
        auto gen = from_range( arr );
        REQUIRE( gen.get() == 5 );
        REQUIRE( gen.next() );
        REQUIRE( gen.get() == 6 );
        REQUIRE( gen.next() );
        REQUIRE( gen.get() == 7 );
        REQUIRE_FALSE( gen.next() );
    }

    SECTION( "ADL range" ) {
        unrelated::needs_ADL_begin<int> range{ 1, 2, 3 };
        auto gen = from_range( range );
        REQUIRE( gen.get() == 1 );
        REQUIRE( gen.next() );
        REQUIRE( gen.get() == 2 );
        REQUIRE( gen.next() );
        REQUIRE( gen.get() == 3 );
        REQUIRE_FALSE( gen.next() );
    }

}

TEST_CASE( "ConcatGenerator", "[generators][concat]" ) {
    using namespace Catch::Generators;
    SECTION( "Cat support single-generator construction" ) {
        ConcatGenerator<int> c( value( 1 ) );
        REQUIRE( c.get() == 1 );
        REQUIRE_FALSE( c.next() );
    }
    SECTION( "Iterating over multiple generators" ) {
        ConcatGenerator<int> c( value( 1 ), values( { 2, 3, 4 } ), value( 5 ) );
        for ( int i = 0; i < 4; ++i ) {
            REQUIRE( c.get() == i + 1 );
            REQUIRE( c.next() );
        }
        REQUIRE( c.get() == 5 );
        REQUIRE_FALSE( c.next() );
    }
}

namespace {
    // Test the default behaviour of skipping generators forward. We do
    // not want to use pre-existing generator, because they will get
    // specialized forward skip implementation.
    class SkipTestGenerator : public Catch::Generators::IGenerator<int> {
        std::vector<int> m_elements{ 0, 1, 2, 3, 4, 5 };
        size_t m_idx = 0;
    public:
        int const& get() const override { return m_elements[m_idx]; }
        bool next() override {
            ++m_idx;
            return m_idx < m_elements.size();
        }

        bool isFinite() const override { return true; }
    };
}

TEST_CASE( "Generators can be skipped forward", "[generators]" ) {
    SkipTestGenerator generator;
    REQUIRE( generator.currentElementIndex() == 0 );

    generator.skipToNthElement( 3 );
    REQUIRE( generator.currentElementIndex() == 3 );
    REQUIRE( generator.get() == 3 );

    // Try "skipping" to the same element.
    generator.skipToNthElement( 3 );
    REQUIRE( generator.currentElementIndex() == 3 );
    REQUIRE( generator.get() == 3 );

    generator.skipToNthElement( 5 );
    REQUIRE( generator.currentElementIndex() == 5 );
    REQUIRE( generator.get() == 5 );

    // Backwards
    REQUIRE_THROWS( generator.skipToNthElement( 3 ) );
    // Past the end
    REQUIRE_THROWS( generator.skipToNthElement( 6 ) );
}

TEST_CASE( "FixedValuesGenerator can be skipped forward",
           "[generators][values]" ) {
    Catch::Generators::FixedValuesGenerator<int> values( {0, 1, 2, 3, 4} );
    REQUIRE( values.currentElementIndex() == 0 );

    values.skipToNthElement( 3 );
    REQUIRE( values.currentElementIndex() == 3 );
    REQUIRE( values.get() == 3 );

    values.skipToNthElement( 4 );
    REQUIRE( values.currentElementIndex() == 4 );
    REQUIRE( values.get() == 4 );

    // Past the end
    REQUIRE_THROWS( values.skipToNthElement( 5 ) );
}

TEST_CASE( "TakeGenerator can be skipped forward", "[generators][take]" ) {
    SECTION("take is shorter than underlying") {
        Catch::Generators::TakeGenerator<int> take(
            6, Catch::Generators::values( { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } ) );
        REQUIRE( take.get() == 0 );

        take.skipToNthElement( 2 );
        REQUIRE( take.get() == 2 );

        take.skipToNthElement( 5 );
        REQUIRE( take.get() == 5 );

        // This is in the original values, but past the end of the take
        REQUIRE_THROWS( take.skipToNthElement( 6 ) );
    }
    SECTION( "take is longer than underlying" ) {
        Catch::Generators::TakeGenerator<int> take(
            8, Catch::Generators::values( { 0, 1, 2, 3, 4, 5 } ) );
        REQUIRE( take.get() == 0 );

        take.skipToNthElement( 2 );
        REQUIRE( take.get() == 2 );

        take.skipToNthElement( 5 );
        REQUIRE( take.get() == 5 );

        // This is in the take, but outside of original values
        REQUIRE_THROWS( take.skipToNthElement( 6 ) );
    }
}

TEST_CASE("MapGenerator can be skipped forward efficiently",
    "[generators][map]") {
    using namespace Catch::Generators;

    int map_calls = 0;
    auto map_func = [&map_calls]( int i ) {
        ++map_calls;
        return i;
    };

    SECTION( "via calls to next()" ) {
        MapGenerator<int, int, decltype( map_func )> map_generator(
            map_func, values( { 0, 1, 2, 3, 4, 5, 6 } ) );
        REQUIRE( map_calls == 0 );

        map_generator.next();
        map_generator.next();
        map_generator.next();
        REQUIRE( map_calls == 0 );
        REQUIRE( map_generator.get() == 3 );
        REQUIRE( map_calls == 1 );
        REQUIRE( map_generator.get() == 3 );
        REQUIRE( map_calls == 1 );

        map_generator.next();
        REQUIRE( map_calls == 1 );
    }
    SECTION("via calls to skipToNthElement()") {
        MapGenerator<int, int, decltype( map_func )> map_generator(
            map_func, values( { 0, 1, 2, 3, 4, 5, 6 } ) );
        REQUIRE( map_calls == 0 );

        map_generator.skipToNthElement( 3 );
        map_generator.skipToNthElement( 4 );
        REQUIRE( map_calls == 0 );
        REQUIRE( map_generator.get() == 4 );
        REQUIRE( map_calls == 1 );

        map_generator.skipToNthElement( 4 );
        REQUIRE( map_generator.get() == 4 );
        REQUIRE( map_calls == 1 );

        map_generator.skipToNthElement( 6 );
        REQUIRE( map_calls == 1 );
        REQUIRE( map_generator.get() == 6 );
        REQUIRE( map_calls == 2 );

        REQUIRE_THROWS( map_generator.skipToNthElement( 7 ) );
        REQUIRE( map_calls == 2 );
    }
}

TEST_CASE( "Generator adapters properly handle isFinite",
           "[generators][map][take][chunk][filter][concat]" ) {
    using namespace Catch::Generators;
    SECTION( "concat generator" ) {
        ConcatGenerator<int> finite_cat(
            value( 1 ), values( { 2, 3, 4 } ), value( 5 ) );
        REQUIRE( finite_cat.isFinite() );

        ConcatGenerator<int> infinite_cat(
            value( 1 ), random( 1, 10 ), value( 3 ) );
        REQUIRE_FALSE( infinite_cat.isFinite() );
    }
    SECTION( "take generator" ) {
        TakeGenerator<int> take_1( 2, values( { 1, 2, 3, 4, 5 } ) );
        REQUIRE( take_1.isFinite() );
        TakeGenerator<int> take_2( 3, random( 1, 100 ) );
        REQUIRE( take_2.isFinite() );
    }
    SECTION( "chunk generator" ) {
        ChunkGenerator<int> finite_chunk( 2, values( { 1, 2, 3, 4, 5 } ) );
        REQUIRE( finite_chunk.isFinite() );
        ChunkGenerator<int> infinite_chunk( 2, random( 1, 100 ) );
        REQUIRE_FALSE( infinite_chunk.isFinite() );
    }
    SECTION( "map" ) {
        auto identity = []( int i ) {
            return i;
        };

        MapGenerator<int, int, decltype( identity )> finite_map(
            identity, values( { 1, 2, 3 } ) );
        REQUIRE( finite_map.isFinite() );
        MapGenerator<int, int, decltype( identity )> infinite_map(
            identity, random( 1, 100 ) );
        REQUIRE_FALSE( infinite_map.isFinite() );
    }
    SECTION( "filter" ) {
        auto always_true = []( int ) {
            return true;
        };
        FilterGenerator<int, decltype( always_true )> finite_filter(
            always_true, values( { 1, 2, 3, 4, 5 } ) );
        REQUIRE( finite_filter.isFinite() );
        FilterGenerator<int, decltype( always_true )> infinite_filter(
            always_true, random( 1, 100 ) );
        REQUIRE_FALSE( infinite_filter.isFinite() );
    }
}

TEST_CASE( "RepeatGenerator refuses infinite generators",
           "[generators][repeat]" ) {
    using namespace Catch::Generators;
    REQUIRE_THROWS( RepeatGenerator<int>( 2, random( 1, 100 ) ) );
}

TEMPLATE_TEST_CASE( "RandomGenerator reports itself as infinite",
                    "[generators][random]",
    int,
    float,
    long double) {
    REQUIRE_FALSE( Catch::Generators::random( TestType{ 0 }, TestType{ 100 } ).isFinite() );
}

namespace {
    struct NotDefaultConstructible {
        int m_i;
        explicit NotDefaultConstructible( int i ): m_i( i ){}
    };
}

TEST_CASE( "MapGenerator can handle not default constructible types",
           "[generators][map]" ) {
    using namespace Catch::Generators;
    auto map_generator = map( []( int i ) { return NotDefaultConstructible( i ); }, values({1, 2, 3}));
    REQUIRE( map_generator.get().m_i == 1 );
    REQUIRE( map_generator.next() );
    REQUIRE( map_generator.next() );
    REQUIRE( map_generator.get().m_i == 3 );
}
