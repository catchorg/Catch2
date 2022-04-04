
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/internal/catch_reporter_spec_parser.hpp>

#include <catch2/interfaces/catch_interfaces_config.hpp>

namespace Catch {

    namespace Detail {
        std::vector<std::string> splitReporterSpec( StringRef reporterSpec ) {
            static constexpr auto separator = "::";
            static constexpr size_t separatorSize = 2;

            size_t separatorPos = 0;
            auto findNextSeparator = [&reporterSpec]( size_t startPos ) {
                static_assert(
                    separatorSize == 2,
                    "The code below currently assumes 2 char separator" );

                auto currentPos = startPos;
                do {
                    while ( currentPos < reporterSpec.size() &&
                            reporterSpec[currentPos] != separator[0] ) {
                        ++currentPos;
                    }
                    if ( currentPos + 1 < reporterSpec.size() &&
                         reporterSpec[currentPos + 1] == separator[1] ) {
                        return currentPos;
                    }
                    ++currentPos;
                } while ( currentPos < reporterSpec.size() );

                return static_cast<size_t>( -1 );
            };

            std::vector<std::string> parts;

            while ( separatorPos < reporterSpec.size() ) {
                const auto nextSeparator = findNextSeparator( separatorPos );
                parts.push_back( static_cast<std::string>( reporterSpec.substr(
                    separatorPos, nextSeparator - separatorPos ) ) );

                if ( nextSeparator == static_cast<size_t>( -1 ) ) {
                    break;
                }
                separatorPos = nextSeparator + separatorSize;
            }

            // Handle a separator at the end.
            // This is not a valid spec, but we want to do validation in a
            // centralized place
            if ( separatorPos == reporterSpec.size() ) {
                parts.emplace_back();
            }

            return parts;
        }

        Optional<ColourMode> stringToColourMode( StringRef colourMode ) {
            if ( colourMode == "default" ) {
                return ColourMode::PlatformDefault;
            } else if ( colourMode == "ansi" ) {
                return ColourMode::ANSI;
            } else if ( colourMode == "win32" ) {
                return ColourMode::Win32;
            } else if ( colourMode == "none" ) {
                return ColourMode::None;
            } else {
                return {};
            }
        }
    } // namespace Detail


} // namespace Catch
