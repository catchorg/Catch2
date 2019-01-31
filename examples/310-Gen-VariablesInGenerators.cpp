// 310-Gen-VariablesInGenerator.cpp
// Shows how to use variables when creating generators.

// Note that using variables inside generators is dangerous and should
// be done only if you know what you are doing, because the generators
// _WILL_ outlive the variables -- thus they should be either captured
// by value directly, or copied by the generators during construction.

#include <catch2/catch.hpp>

#include <random>

// Lets start by implementing a parametrizable double generator
class RandomDoubleGenerator : public Catch::Generators::IGenerator<double> {
    std::minstd_rand m_rand;
    std::uniform_real_distribution<> m_dist;
    double current_number;
public:

    RandomDoubleGenerator(double low, double high):
        m_rand(std::random_device{}()),
        m_dist(low, high)
    {
        static_cast<void>(next());
    }

    double const& get() const override;
    bool next() override {
        current_number = m_dist(m_rand);
        return true;
    }
};

// Avoids -Wweak-vtables
double const& RandomDoubleGenerator::get() const {
    return current_number;
}


// Also provide a nice shortcut for creating the generator
Catch::Generators::GeneratorWrapper<double> random(double low, double high) {
    return Catch::Generators::GeneratorWrapper<double>(std::unique_ptr<Catch::Generators::IGenerator<double>>(new RandomDoubleGenerator(low, high)));
}


TEST_CASE("Generate random doubles across different ranges",
          "[generator][example][advanced]") {
    // Workaround for old libstdc++
    using record = std::tuple<double, double>;
    // Set up 3 ranges to generate numbers from
    auto r = GENERATE(table<double, double>({
        record{3, 4},
        record{-4, -3},
        record{10, 1000}
    }));

    // This will not compile (intentionally), because it accesses a variable
    // auto number = GENERATE(take(50, random(r.first, r.second)));
    
    // We have to manually register the generators instead
    // Notice that we are using value capture in the lambda, to avoid lifetime issues
    auto number = Catch::Generators::generate( CATCH_INTERNAL_LINEINFO,
        [=]{
            using namespace Catch::Generators;
            return makeGenerators(take(50, random(std::get<0>(r), std::get<1>(r))));
        }
    );
    REQUIRE(std::abs(number) > 0);
}

// Compiling and running this file will result in 150 successful assertions

