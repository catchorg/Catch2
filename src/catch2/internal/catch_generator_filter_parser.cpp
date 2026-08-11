
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_generator_filter_parser.hpp>

#include <catch2/internal/catch_parse_numbers.hpp>
#include <catch2/internal/catch_string_manip.hpp>

#include <string>

namespace Catch {

    namespace {

        GeneratorFilterParseResult error( std::string message ) {
            GeneratorFilterParseResult result;
            result.type = GeneratorFilterParseResult::Type::Error;
            result.errorMessage = CATCH_MOVE( message );
            return result;
        }

        Optional<unsigned int> parseSingleIndex( std::string const& token ) {
            if ( token.empty() ) { return {}; }
            return parseUInt( token );
        }

        GeneratorFilterParseResult parseRangeToken( std::string const& token ) {
            auto const dashPos = token.find( '-' );
            if ( dashPos == std::string::npos ) {
                auto parsed = parseSingleIndex( token );
                if ( !parsed ) {
                    return error( "Could not parse '" + token +
                                    "' as generator index" );
                }
                GeneratorFilterParseResult result;
                result.type = GeneratorFilterParseResult::Type::Ok;
                result.indices.push_back( *parsed );
                return result;
            }

            if ( dashPos == 0 || dashPos + 1 == token.size() ) {
                return error( "Could not parse '" + token +
                                "' as generator index range" );
            }

            auto const startToken = token.substr( 0, dashPos );
            auto const endToken = token.substr( dashPos + 1 );
            if ( endToken.find( '-' ) != std::string::npos ) {
                return error( "Could not parse '" + token +
                                "' as generator index range" );
            }

            auto const start = parseSingleIndex( startToken );
            auto const end = parseSingleIndex( endToken );
            if ( !start || !end ) {
                return error( "Could not parse '" + token +
                                "' as generator index range" );
            }
            if ( *start > *end ) {
                return error( "Generator index range '" + token +
                                "' must be monotonically increasing" );
            }

            GeneratorFilterParseResult result;
            result.type = GeneratorFilterParseResult::Type::Ok;
            for ( unsigned int index = *start; index <= *end; ++index ) {
                result.indices.push_back( index );
            }
            return result;
        }

    } // namespace

    GeneratorFilterParseResult parseGeneratorIndexSpec( std::string const& input ) {
        auto const trimmed = trim( input );
        if ( trimmed.empty() ) {
            return error( "Generator index spec cannot be empty" );
        }

        GeneratorFilterParseResult result;
        result.type = GeneratorFilterParseResult::Type::Ok;

        std::string token;
        for ( char ch : trimmed ) {
            if ( ch == ',' ) {
                if ( token.empty() ) {
                    return error( "Could not parse '" + trimmed +
                                    "' as generator index spec" );
                }
                auto parsedToken = parseRangeToken( token );
                if ( parsedToken.type == GeneratorFilterParseResult::Type::Error ) {
                    return parsedToken;
                }
                result.indices.insert( result.indices.end(),
                                       parsedToken.indices.begin(),
                                       parsedToken.indices.end() );
                token.clear();
            } else {
                token.push_back( ch );
            }
        }

        if ( token.empty() ) {
            return error( "Could not parse '" + trimmed +
                            "' as generator index spec" );
        }

        auto parsedToken = parseRangeToken( token );
        if ( parsedToken.type == GeneratorFilterParseResult::Type::Error ) {
            return parsedToken;
        }
        result.indices.insert( result.indices.end(),
                               parsedToken.indices.begin(),
                               parsedToken.indices.end() );

        if ( result.indices.empty() ) {
            return error( "Could not parse '" + trimmed +
                            "' as generator index spec" );
        }

        for ( std::size_t i = 1; i < result.indices.size(); ++i ) {
            if ( result.indices[i] <= result.indices[i - 1] ) {
                return error( "Generator index spec '" + trimmed +
                                "' must be monotonically increasing" );
            }
        }

        return result;
    }

} // namespace Catch
