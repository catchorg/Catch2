/*
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/internal/catch_tag_alias_registry.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_test_case_info.hpp>

TEST_CASE( "Tag alias can be registered against tag patterns" ) {

    Catch::TagAliasRegistry registry;

    registry.add( "[@zzz]", "[one][two]", Catch::SourceLineInfo( "file", 2 ) );

    SECTION( "The same tag alias can only be registered once" ) {

        try {
            registry.add( "[@zzz]", "[one][two]", Catch::SourceLineInfo( "file", 10 ) );
            FAIL( "expected exception" );
        }
        catch( std::exception& ex ) {
            std::string what = ex.what();
            using namespace Catch::Matchers;
            CHECK_THAT( what, Contains( "[@zzz]" ) );
            CHECK_THAT( what, Contains( "file" ) );
            CHECK_THAT( what, Contains( "2" ) );
            CHECK_THAT( what, Contains( "10" ) );
        }
    }

    SECTION( "Tag aliases must be of the form [@name]" ) {
        CHECK_THROWS( registry.add( "[no ampersat]", "", Catch::SourceLineInfo( "file", 3 ) ) );
        CHECK_THROWS( registry.add( "[the @ is not at the start]", "", Catch::SourceLineInfo( "file", 3 ) ) );
        CHECK_THROWS( registry.add( "@no square bracket at start]", "", Catch::SourceLineInfo( "file", 3 ) ) );
        CHECK_THROWS( registry.add( "[@no square bracket at end", "", Catch::SourceLineInfo( "file", 3 ) ) );
    }
}

TEST_CASE("shortened hide tags are split apart") {
    using Catch::StringRef;
    using Catch::Matchers::VectorContains;
    auto testcase = Catch::makeTestCaseInfo("", {"fake test name", "[.magic-tag]"}, CATCH_INTERNAL_LINEINFO);
    // Transform ...
    std::vector<StringRef> tags;
    for (auto const& tag : testcase->tags) {
        tags.push_back(tag.original);
    }
    REQUIRE_THAT(tags, VectorContains("magic-tag"_catch_sr) && VectorContains("."_catch_sr));
}
