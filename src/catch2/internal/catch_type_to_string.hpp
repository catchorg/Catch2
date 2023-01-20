
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TYPE_TO_STRING_HPP_INCLUDED
#define CATCH_TYPE_TO_STRING_HPP_INCLUDED

#include <typeinfo>

#if defined( CATCH_INTERNAL_TYPEID_DEMANGLE )
#    include <memory>
#    include <cxxabi.h>
#endif

namespace Catch {
    template <typename T> std::string type_to_string() {
#if defined( CATCH_INTERNAL_TYPEID_DEMANGLE )
        const char* name = typeid( T ).name();
        int status = 0;
        std::unique_ptr<char, void ( * )( void* )> res{
            abi::__cxa_demangle( name, NULL, NULL, &status ), std::free };

        return status == 0 ? res.get() : name;
#else
        return typeid( T ).name();
#endif
    }
} // end namespace Catch

#endif // CATCH_TYPE_TO_STRING_HPP_INCLUDED
