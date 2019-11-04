#include "internal/catch_stringref.h"

#include "catch.hpp"

#include <cstring>

TEST_CASE( "StringRef", "[Strings][StringRef]" ) {

    using Catch::StringRef;

    SECTION( "Empty string" ) {
        StringRef empty;
        REQUIRE( empty.empty() );
        REQUIRE( empty.size() == 0 );
        REQUIRE( empty.isNullTerminated() );
        REQUIRE( std::strcmp( empty.c_str(), "" ) == 0 );
    }

    SECTION( "From string literal" ) {
        StringRef s = "hello";
        REQUIRE( s.empty() == false );
        REQUIRE( s.size() == 5 );
        REQUIRE( s.isNullTerminated() );

        auto rawChars = s.data();
        REQUIRE( std::strcmp( rawChars, "hello" ) == 0 );

        REQUIRE_NOTHROW(s.c_str());
        REQUIRE(s.c_str() == rawChars);
        REQUIRE(s.data() == rawChars);
    }
    SECTION( "From sub-string" ) {
        StringRef original = StringRef( "original string" ).substr(0, 8);
        REQUIRE( original == "original" );

        REQUIRE_FALSE(original.isNullTerminated());
        REQUIRE_THROWS(original.c_str());
        REQUIRE_NOTHROW(original.data());
    }


    SECTION( "Substrings" ) {
        StringRef s = "hello world!";
        StringRef ss = s.substr(0, 5);

        SECTION( "zero-based substring" ) {
            REQUIRE( ss.empty() == false );
            REQUIRE( ss.size() == 5 );
            REQUIRE( std::strncmp( ss.data(), "hello", 5 ) == 0 );
            REQUIRE( ss == "hello" );
        }

        SECTION( "non-zero-based substring") {
            ss = s.substr( 6, 6 );
            REQUIRE( ss.size() == 6 );
            REQUIRE( std::strcmp( ss.c_str(), "world!" ) == 0 );
        }

        SECTION( "Pointer values of full refs should match" ) {
            StringRef s2 = s;
            REQUIRE( s.data() == s2.data() );
        }

        SECTION( "Pointer values of substring refs should also match" ) {
            REQUIRE( s.data() == ss.data() );
        }

        SECTION("Past the end substring") {
            REQUIRE(s.substr(s.size() + 1, 123).empty());
        }

        SECTION("Substring off the end are trimmed") {
            ss = s.substr(6, 123);
            REQUIRE(std::strcmp(ss.c_str(), "world!") == 0);
        }
        // TODO: substring into string + size is longer than end
    }

    SECTION( "Comparisons are deep" ) {
        char buffer1[] = "Hello";
        char buffer2[] = "Hello";
        CHECK((char*)buffer1 != (char*)buffer2);

        StringRef left(buffer1), right(buffer2);
        REQUIRE( left == right );
        REQUIRE(left != left.substr(0, 3));
    }

    SECTION( "from std::string" ) {
        std::string stdStr = "a standard string";

        SECTION( "implicitly constructed" ) {
            StringRef sr = stdStr;
            REQUIRE( sr == "a standard string" );
            REQUIRE( sr.size() == stdStr.size() );
        }
        SECTION( "explicitly constructed" ) {
            StringRef sr( stdStr );
            REQUIRE( sr == "a standard string" );
            REQUIRE( sr.size() == stdStr.size() );
        }
        SECTION( "assigned" ) {
            StringRef sr;
            sr = stdStr;
            REQUIRE( sr == "a standard string" );
            REQUIRE( sr.size() == stdStr.size() );
        }
    }

    SECTION( "to std::string" ) {
        StringRef sr = "a stringref";

        SECTION( "explicitly constructed" ) {
            std::string stdStr( sr );
            REQUIRE( stdStr == "a stringref" );
            REQUIRE( stdStr.size() == sr.size() );
        }
        SECTION( "assigned" ) {
            std::string stdStr;
            stdStr = static_cast<std::string>(sr);
            REQUIRE( stdStr == "a stringref" );
            REQUIRE( stdStr.size() == sr.size() );
        }
    }
}

TEST_CASE("StringRef at compilation time", "[Strings][StringRef][constexpr]") {
    using Catch::StringRef;
    SECTION("Simple constructors") {
        STATIC_REQUIRE(StringRef{}.size() == 0);

        STATIC_REQUIRE(StringRef{ "abc", 3 }.size() == 3);
        STATIC_REQUIRE(StringRef{ "abc", 3 }.isNullTerminated());

        STATIC_REQUIRE(StringRef{ "abc", 2 }.size() == 2);
        STATIC_REQUIRE_FALSE(StringRef{ "abc", 2 }.isNullTerminated());
    }
    SECTION("UDL construction") {
        constexpr auto sr1 = "abc"_catch_sr;
        STATIC_REQUIRE_FALSE(sr1.empty());
        STATIC_REQUIRE(sr1.size() == 3);
        STATIC_REQUIRE(sr1.isNullTerminated());

        using Catch::operator"" _sr;
        constexpr auto sr2 = ""_sr;
        STATIC_REQUIRE(sr2.empty());
        STATIC_REQUIRE(sr2.size() == 0);
        STATIC_REQUIRE(sr2.isNullTerminated());
    }
}
