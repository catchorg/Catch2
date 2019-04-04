/*
 *  Created by Phil on 4/4/2019.
 *  Copyright 2019 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "catch_enum_values_registry.h"
#include "catch_string_manip.h"
#include "catch_stream.h"

#include <map>

namespace Catch {

    IMutableEnumValuesRegistry::~IMutableEnumValuesRegistry() {}
    IEnumInfo::~IEnumInfo() {}

    namespace Detail {

        std::vector<std::string> parseEnums( StringRef enums ) {
            auto enumValues = splitString( enums, ',' );
            std::vector<std::string> parsed;
            parsed.reserve( enumValues.size() );
            for( auto const& enumValue : enumValues ) {
                auto identifiers = splitString( enumValue, ':' );
                parsed.push_back( Catch::trim( identifiers.back() ) );
            }
            return parsed;
        }

        struct EnumInfo : IEnumInfo {
            std::string m_name;
            std::map<int, std::string> m_values;

            ~EnumInfo();

            std::string lookup( int value ) const override {
                auto it = m_values.find( value );
                if( it == m_values.end() ) {
                    ReusableStringStream rss;
                    rss << "{** unexpected value for " << m_name << ": " << value << "**}";
                    return rss.str();
                }
                return it->second;
            }
        };
        EnumInfo::~EnumInfo() {}

        IEnumInfo const& EnumValuesRegistry::registerEnum( StringRef enumName, StringRef allValueNames, std::vector<int> const& values ) {
            std::unique_ptr<EnumInfo> enumInfo( new EnumInfo );
            enumInfo->m_name = enumName;

            const auto valueNames = Catch::Detail::parseEnums( allValueNames );
            assert( valueNames.size() == values.size() );
            std::size_t i = 0;
            for( auto value : values )
                enumInfo->m_values.insert({ value, valueNames[i++] });

            EnumInfo* raw = enumInfo.get();
            m_enumInfos.push_back( std::move( enumInfo ) );
            return *raw;
        }

    } // Detail

} // Catch

