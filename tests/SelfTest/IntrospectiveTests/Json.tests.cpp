
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_jsonwriter.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <sstream>

namespace {
    struct Custom {};
    std::ostream& operator<<( std::ostream& os, Custom const& ) {
        return os << "custom";
    }
} // namespace

TEST_CASE( "JsonWriter", "[JSON][JsonWriter]" ) {

    std::stringstream stream{};
    SECTION( "Newly constructed JsonWriter does nothing" ) {
        Catch::JsonValueWriter writer{ stream };
        REQUIRE( stream.str() == "" );
    }

    SECTION( "Calling writeObject will create an empty pair of braces" ) {
        { auto writer = Catch::JsonValueWriter{ stream }.writeObject(); }
        REQUIRE( stream.str() == "{\n}" );
    }

    SECTION( "Calling writeObject with key will create an object to write the "
             "value" ) {
        using Catch::Matchers::ContainsSubstring;
        {
            auto writer = Catch::JsonValueWriter{ stream }.writeObject();
            writer.write( "int" ).write( 1 );
            writer.write( "double" ).write( 1.5 );
            writer.write( "true" ).write( true );
            writer.write( "false" ).write( false );
            writer.write( "string" ).write( "this is a string" );
            writer.write( "array" ).writeArray().write( 1 ).write( 2 );
        }
        REQUIRE_THAT(
            stream.str(),
            ContainsSubstring( "\"int\": 1," ) &&
                ContainsSubstring( "\"double\": 1.5," ) &&
                ContainsSubstring( "\"true\": true," ) &&
                ContainsSubstring( "\"false\": false," ) &&
                ContainsSubstring( "\"string\": \"this is a string\"," ) &&
                ContainsSubstring( "\"array\": [\n    1,\n    2\n  ]\n}" ) );
    }

    SECTION( "nesting objects" ) {
        using Catch::Matchers::ContainsSubstring;
        {
            auto writer = Catch::JsonValueWriter{ stream }.writeObject();
            writer.write( "empty_object" ).writeObject();
            writer.write( "fully_object" )
                .writeObject()
                .write( "key" )
                .write( 1 );
        }
        REQUIRE_THAT( stream.str(),
                      ContainsSubstring( "\"empty_object\": {\n  }," ) &&
                          ContainsSubstring(
                              "\"fully_object\": {\n    \"key\": 1\n  }" ) );
    }

    SECTION( "Calling writeArray will create an empty pair of braces" ) {
        { auto writer = Catch::JsonValueWriter{ stream }.writeArray(); }
        REQUIRE( stream.str() == "[\n]" );
    }

    SECTION( "Calling writeArray creates array to write the values to" ) {
        {
            auto writer = Catch::JsonValueWriter{ stream }.writeArray();
            writer.write( 1 );
            writer.write( 1.5 );
            writer.write( true );
            writer.write( false );
            writer.write( "this is a string" );
            writer.writeObject().write( "object" ).write( 42 );
            writer.writeArray().write( "array" ).write( 42.5 );
        }
        REQUIRE( stream.str() == "[\n  1,\n  1.5,\n  true,\n  false,\n  \"this is a string\",\n  {\n    \"object\": 42\n  },\n  [\n    \"array\",\n    42.5\n  ]\n]" );
    }

    SECTION(
        "Moved from JsonObjectWriter shall not insert superfluous brace" ) {
        {
            auto writer = Catch::JsonObjectWriter{ stream };
            auto another_writer = std::move( writer );
        }
        REQUIRE( stream.str() == "{\n}" );
    }
    SECTION(
        "Moved from JsonArrayWriter shall not insert superfluous bracket" ) {
        {
            auto writer = Catch::JsonArrayWriter{ stream };
            auto another_writer = std::move( writer );
        }
        REQUIRE( stream.str() == "[\n]" );
    }
    SECTION( "Custom class shall be quoted" ) {
        Catch::JsonValueWriter{ stream }.write( Custom{} );
        REQUIRE( stream.str() == "\"custom\"" );
    }
    SECTION( "String with a quote shall be espaced" ) {
        Catch::JsonValueWriter{ stream }.write( "\"" );
        REQUIRE( stream.str() == "\"\\\"\"" );
    }
}
