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
#include <cassert>

namespace Catch {

    IMutableEnumValuesRegistry::~IMutableEnumValuesRegistry() {}

    namespace Detail {

        std::vector<std::string> parseEnums( StringRef enums ) {
            auto enumValues = splitStringRef( enums, ',' );
            std::vector<std::string> parsed;
            parsed.reserve( enumValues.size() );
            for( auto const& enumValue : enumValues ) {
                auto identifiers = splitStringRef( enumValue, ':' );
                parsed.push_back( Catch::trim( identifiers.back() ) );
            }
            return parsed;
        }

        EnumInfo::~EnumInfo() {}

        StringRef EnumInfo::lookup( int value ) const {
            for( auto const& valueToName : m_values ) {
                if( valueToName.first == value )
                    return valueToName.second;
            }
            return "{** unexpected enum value **}";
        }

        std::unique_ptr<EnumInfo> makeEnumInfo( StringRef enumName, StringRef allValueNames, std::vector<int> const& values ) {
            std::unique_ptr<EnumInfo> enumInfo( new EnumInfo );
            enumInfo->m_name = enumName;
            enumInfo->m_values.reserve( values.size() );

            const auto valueNames = Catch::Detail::parseEnums( allValueNames );
            assert( valueNames.size() == values.size() );
            std::size_t i = 0;
            for( auto value : values )
                enumInfo->m_values.push_back({ value, valueNames[i++] });

            return enumInfo;
        }

        EnumInfo const& EnumValuesRegistry::registerEnum( StringRef enumName, StringRef allValueNames, std::vector<int> const& values ) {
            auto enumInfo = makeEnumInfo( enumName, allValueNames, values );
            EnumInfo* raw = enumInfo.get();
            m_enumInfos.push_back( std::move( enumInfo ) );
            return *raw;
        }

    } // Detail
} // Catch

