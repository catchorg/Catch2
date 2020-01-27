// X20-BenchmarkingMacros.cpp
// Test that the benchmarking support macros compile properly with the single header

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace {
std::uint64_t factorial(std::uint64_t number) {
    if (number < 2) {
        return 1;
    }
    return number * factorial(number - 1);
}
}

TEST_CASE("Benchmark factorial", "[benchmark]") {
    CHECK(factorial(0) == 1);
    // some more asserts..
    CHECK(factorial(10) == 3628800);

    BENCHMARK("factorial 10") {
        return factorial(10);
    };

    CHECK(factorial(14) == 87178291200ull);
    BENCHMARK("factorial 14") {
        return factorial(14);
    };
}

TEST_CASE("Benchmark containers", "[.][benchmark]") {
    static const int size = 100;

    std::vector<int> v;
    std::map<int, int> m;

    SECTION("without generator") {
        BENCHMARK("Load up a vector") {
            v = std::vector<int>();
            for (int i = 0; i < size; ++i)
                v.push_back(i);
        };
        REQUIRE(v.size() == size);

        // test optimizer control
        BENCHMARK("Add up a vector's content") {
            uint64_t add = 0;
            for (int i = 0; i < size; ++i)
                add += v[i];
            return add;
        };

        BENCHMARK("Load up a map") {
            m = std::map<int, int>();
            for (int i = 0; i < size; ++i)
                m.insert({ i, i + 1 });
        };
        REQUIRE(m.size() == size);

        BENCHMARK("Reserved vector") {
            v = std::vector<int>();
            v.reserve(size);
            for (int i = 0; i < size; ++i)
                v.push_back(i);
        };
        REQUIRE(v.size() == size);

        BENCHMARK("Resized vector") {
            v = std::vector<int>();
            v.resize(size);
            for (int i = 0; i < size; ++i)
                v[i] = i;
        };
        REQUIRE(v.size() == size);

        int array[size];
        BENCHMARK("A fixed size array that should require no allocations") {
            for (int i = 0; i < size; ++i)
                array[i] = i;
        };
        int sum = 0;
        for (int i = 0; i < size; ++i)
            sum += array[i];
        REQUIRE(sum > size);

        SECTION("XYZ") {

            BENCHMARK_ADVANCED("Load up vector with chronometer")(Catch::Benchmark::Chronometer meter) {
                std::vector<int> k;
                meter.measure([&](int idx) {
                    k = std::vector<int>();
                    for (int i = 0; i < size; ++i)
                        k.push_back(idx);
                });
                REQUIRE(k.size() == size);
            };

            int runs = 0;
            BENCHMARK("Fill vector indexed", benchmarkIndex) {
                v = std::vector<int>();
                v.resize(size);
                for (int i = 0; i < size; ++i)
                    v[i] = benchmarkIndex;
                runs = benchmarkIndex;
            };

            for (size_t i = 0; i < v.size(); ++i) {
                REQUIRE(v[i] == runs);
            }
        }
    }

    SECTION("with generator") {
        auto generated = GENERATE(range(0, 10));
        BENCHMARK("Fill vector generated") {
            v = std::vector<int>();
            v.resize(size);
            for (int i = 0; i < size; ++i)
                v[i] = generated;
        };
        for (size_t i = 0; i < v.size(); ++i) {
            REQUIRE(v[i] == generated);
        }
    }
}
