#include <catch2/catch_config.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_clara.hpp>
#include <catch2/internal/catch_commandline.hpp>
#include <catch2/internal/catch_tag_alias_registry.hpp>
#include <catch2/internal/catch_template_test_registry.hpp>
#include <catch2/internal/catch_test_case_info_hasher.hpp>
#include <catch2/internal/catch_test_case_registry_impl.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>

#include <iostream>

static constexpr Catch::SourceLineInfo dummySourceLineInfo = CATCH_INTERNAL_LINEINFO;

TEST_CASE( "Set TestRunOrder of command line parser to Randomized and register tests in registry." ) {
    Catch::ConfigData configData;
    auto cli = Catch::makeCommandLineParser(configData);
    CHECK(cli.parse({"test", "--order", "random"}));
    REQUIRE(configData.runOrder == Catch::TestRunOrder::Randomized);

    Catch::StringRef className1{std::string("name")};
    Catch::NameAndTags nameAndTag1{std::string("nameA"), std::string("[tagA]")};
    auto ptrToTestCaseInfo1 = Catch::Detail::make_unique<Catch::TestCaseInfo>(className1, nameAndTag1, dummySourceLineInfo);
    auto ptrToTestInvoker1 = Catch::Detail::make_unique<Catch::TestInvokerAsFunction>( []{std::cout << "f1\n";} );

    Catch::StringRef className2{std::string("nameX")};
    Catch::NameAndTags nameAndTag2{std::string("nameA"), std::string("[tagA]")};

    auto ptrToTestCaseInfo2 = Catch::Detail::make_unique<Catch::TestCaseInfo>(className2, nameAndTag2, dummySourceLineInfo);
    auto ptrToTestInvoker2 = Catch::Detail::make_unique<Catch::TestInvokerAsFunction>( []{std::cout << "f2\n";} );

    Catch::StringRef className3{std::string("name")};
    Catch::NameAndTags nameAndTag3{std::string("nameA"), std::string("[tagB]")};

    auto ptrToTestCaseInfo3 = Catch::Detail::make_unique<Catch::TestCaseInfo>(className3, nameAndTag3, dummySourceLineInfo);
    auto ptrToTestInvoker3 = Catch::Detail::make_unique<Catch::TestInvokerAsFunction>( []{std::cout << "f3\n";} );

    Catch::TestRegistry testRegistry;
    testRegistry.registerTest(std::move(ptrToTestCaseInfo1), std::move(ptrToTestInvoker1));
    testRegistry.registerTest(std::move(ptrToTestCaseInfo2), std::move(ptrToTestInvoker2));

    Catch::Config config(configData);
    const auto& allTests = testRegistry.getAllTestsSorted(config);

    for (const auto& test : allTests)
    {
        test.invoke();
    }
}
