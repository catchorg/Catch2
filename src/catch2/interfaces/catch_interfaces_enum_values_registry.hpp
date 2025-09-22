
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_INTERFACES_ENUM_VALUES_REGISTRY_HPP_INCLUDED
#define CATCH_INTERFACES_ENUM_VALUES_REGISTRY_HPP_INCLUDED

#include <string_view>
#include <vector>

namespace Catch {

    namespace Detail {
        struct EnumInfo {
            std::string_view m_name;
            std::vector<std::pair<int, std::string_view>> m_values;

            ~EnumInfo();

            std::string_view lookup( int value ) const;
        };
    } // namespace Detail

    class IMutableEnumValuesRegistry {
    public:
        virtual ~IMutableEnumValuesRegistry(); // = default;

        virtual Detail::EnumInfo const& registerEnum( std::string_view enumName, std::string_view allEnums, std::vector<int> const& values ) = 0;

        template<typename E>
        Detail::EnumInfo const& registerEnum( std::string_view enumName, std::string_view allEnums, std::initializer_list<E> values ) {
            static_assert(sizeof(int) >= sizeof(E), "Cannot serialize enum to int");
            std::vector<int> intValues;
            intValues.reserve( values.size() );
            for( auto enumValue : values )
                intValues.push_back( static_cast<int>( enumValue ) );
            return registerEnum( enumName, allEnums, intValues );
        }
    };

} // Catch

#endif // CATCH_INTERFACES_ENUM_VALUES_REGISTRY_HPP_INCLUDED
