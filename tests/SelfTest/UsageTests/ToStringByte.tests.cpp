#include <catch2/catch_test_macros.hpp>

#if defined(CATCH_CONFIG_CPP17_BYTE)

TEST_CASE( "std::byte -> toString", "[toString][byte][approvals]" ) {
    using type = std::byte;
    REQUIRE( "0" == ::Catch::Detail::stringify( type{ 0 } ) );
}

TEST_CASE( "std::vector<std::byte> -> toString", "[toString][byte][approvals]" ) {
    using type = std::vector<std::byte>;
    REQUIRE( "{ 0, 1, 2 }" == ::Catch::Detail::stringify( type{ std::byte{0}, std::byte{1}, std::byte{2} } ) );
}

#endif // CATCH_INTERNAL_CONFIG_CPP17_BYTE
