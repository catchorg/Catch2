/*
 *  Created by Phil on 4/4/2019.
 *  Copyright 2019 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_INTERFACESENUMVALUESREGISTRY_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACESENUMVALUESREGISTRY_H_INCLUDED

#include "catch_stringref.h"

#include <vector>

namespace Catch {

    namespace Detail {
        struct EnumInfo {
            StringRef m_name;
            std::vector<std::pair<int, std::string>> m_values;

            ~EnumInfo();

            StringRef lookup( int value ) const;
        };
    } // namespace Detail

    struct IMutableEnumValuesRegistry {
        virtual ~IMutableEnumValuesRegistry();

        virtual Detail::EnumInfo const& registerEnum( StringRef enumName, StringRef allEnums, std::vector<int> const& values ) = 0;

        template<typename E>
        Detail::EnumInfo const& registerEnum( StringRef enumName, StringRef allEnums, std::initializer_list<E> values ) {
            std::vector<int> intValues;
            intValues.reserve( values.size() );
            for( auto enumValue : values )
                intValues.push_back( static_cast<int>( enumValue ) );
            return registerEnum( enumName, allEnums, intValues );
        }
    };

} // Catch

#endif //TWOBLUECUBES_CATCH_INTERFACESENUMVALUESREGISTRY_H_INCLUDED
