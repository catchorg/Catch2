//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_jsonwriter.hpp>

#include <sstream>

namespace {
    struct Custom {};
    std::ostream& operator<<( std::ostream& os, Custom const& ) {
        return os << "custom";
    }
} // namespace

TEST_CASE( "JsonWriter", "[JSON][JsonWriter]" ) {

    std::stringstream stream{};
    stream << "\n"; // Make the expected json nicer to look at
    SECTION( "Newly constructed JsonWriter does nothing" ) {
        Catch::JsonValueWriter writer{ stream };
        REQUIRE( stream.str() == R"json(
)json" );
    }

    SECTION( "Calling writeObject will create an empty pair of braces" ) {
        { auto writer = Catch::JsonValueWriter{ stream }.writeObject(); }
        REQUIRE( stream.str() == R"json(
{
})json" );
    }

    SECTION( "Calling writeObject with key will create an object to write the "
             "value" ) {
        {
            auto writer = Catch::JsonValueWriter{ stream }.writeObject();
            writer.write( "int" ).write( 1 );
            writer.write( "double" ).write( 1.5 );
            writer.write( "true" ).write( true );
            writer.write( "false" ).write( false );
            writer.write( "string" ).write( "this is a string" );
            writer.write( "array" ).writeArray().write( 1 ).write( 2 );
        }
        REQUIRE( stream.str() == R"json(
{
  "int": 1,
  "double": 1.5,
  "true": true,
  "false": false,
  "string": "this is a string",
  "array": [
    1,
    2
  ]
})json" );
    }

    SECTION( "nesting objects" ) {
        {
            auto writer = Catch::JsonValueWriter{ stream }.writeObject();
            writer.write( "empty_object" ).writeObject();
            writer.write( "fully_object" )
                .writeObject()
                .write( "key" )
                .write( 1 );
        }
        REQUIRE( stream.str() == R"json(
{
  "empty_object": {
  },
  "fully_object": {
    "key": 1
  }
})json" );
    }

    SECTION( "Calling writeArray will create an empty pair of braces" ) {
        { auto writer = Catch::JsonValueWriter{ stream }.writeArray(); }
        REQUIRE( stream.str() == R"json(
[
])json" );
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
        REQUIRE( stream.str() == R"json(
[
  1,
  1.5,
  true,
  false,
  "this is a string",
  {
    "object": 42
  },
  [
    "array",
    42.5
  ]
])json" );
    }

    SECTION(
        "Moved from JsonObjectWriter shall not insert superfluous brace" ) {
        {
            auto writer = Catch::JsonObjectWriter{ stream };
            auto another_writer = std::move( writer );
        }
        REQUIRE( stream.str() == R"json(
{
})json" );
    }
    SECTION(
        "Moved from JsonArrayWriter shall not insert superfluous bracket" ) {
        {
            auto writer = Catch::JsonArrayWriter{ stream };
            auto another_writer = std::move( writer );
        }
        REQUIRE( stream.str() == R"json(
[
])json" );
    }
    SECTION( "Custom class shall be quoted" ) {
        Catch::JsonValueWriter{ stream }.write( Custom{} );
        REQUIRE( stream.str() == R"json(
"custom")json" );
    }
    SECTION( "String with a quote shall be espaced" ) {
        Catch::JsonValueWriter{ stream }.write( "\"" );
        REQUIRE( stream.str() == "\n\"\\\"\"" );
    }
}
