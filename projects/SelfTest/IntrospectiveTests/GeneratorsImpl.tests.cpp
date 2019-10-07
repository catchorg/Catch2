#include "catch.hpp"

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
        auto gen = filter([] (int i) { return i != 2; }, values({ 2, 1, 2, 3, 2, 2 }));
        REQUIRE(gen.get() == 1);
        REQUIRE(gen.next());
        REQUIRE(gen.get() == 3);
        REQUIRE_FALSE(gen.next());

        // Completely filtered-out generator should throw on construction
        REQUIRE_THROWS_AS(filter([] (int) { return false; }, value(1)), Catch::GeneratorException);
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
    auto value2 = GENERATE_REF(Catch::Generators::GeneratorWrapper<int>(std::unique_ptr<Catch::Generators::IGenerator<int>>(new TestGen(nc))));
    REQUIRE(value == value2);
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
        // 0.5% seems like a sane bound for random identical elements within 1000 runs
        REQUIRE(same < 5);
    }
    SECTION("Float") {
        auto random1 = Catch::Generators::random(0., 1000.);
        auto random2 = Catch::Generators::random(0., 1000.);
        size_t same = 0;
        for (size_t i = 0; i < 1000; ++i) {
            same += random1.get() == random2.get();
            random1.next(); random2.next();
        }
        // 0.5% seems like a sane bound for random identical elements within 1000 runs
        REQUIRE(same < 5);
    }
}
