// 301-Gen-MapTypeConversion.cpp
// Shows how to use map to modify generator's return type.

// TODO

#include <catch2/catch.hpp>

#include <string>
#include <sstream>

// Returns a line from a stream. You could have it e.g. read lines from
// a file, but to avoid problems with paths in examples, we will use
// a fixed stringstream.
class LineGenerator : public Catch::Generators::IGenerator<std::string> {
    std::string m_line;
    std::stringstream m_stream;
public:
    LineGenerator() {
        m_stream.str("1\n2\n3\n4\n");
        if (!next()) {
            throw Catch::GeneratorException("Couldn't read a single line");
        }
    }

    std::string const& get() const override {
        return m_line;
    }
    
    bool next() override {
        return !!std::getline(m_stream, m_line);
    }
};

// This helper function provides a nicer UX when instantiating the generator
// Notice that it returns an instance of GeneratorWrapper<std::string>, which
// is a value-wrapper around std::unique_ptr<IGenerator<std::string>>.
Catch::Generators::GeneratorWrapper<std::string> lines(std::string /* ignored for example */) {
    return Catch::Generators::GeneratorWrapper<std::string>(
        std::unique_ptr<Catch::Generators::IGenerator<std::string>>(
            new LineGenerator()
        )
    );
}



TEST_CASE("filter can convert types inside the generator expression", "[example][generator]") {
    auto num = GENERATE(map<int>([](std::string const& line) { return std::stoi(line); },
                                 lines("fake-file")));
                                 
    REQUIRE(num > 0);
}

// Compiling and running this file will result in 4 successful assertions
