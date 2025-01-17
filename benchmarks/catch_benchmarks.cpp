#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <mutex>

std::recursive_mutex global_lock;

int no_lock() {
    return 2;
}

int take_lock() {
    std::unique_lock<std::recursive_mutex> lock(global_lock);
    return 2;
}

TEST_CASE("std::recursive_mutex overhead benchmark", "[benchmark][mutex]") {
    BENCHMARK("no lock") {
        return no_lock();
    };

    BENCHMARK("with std::recursive_mutex") {
        return take_lock();
    };
}
