
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#ifndef CATCH_FLAG_ENUM_HPP_INCLUDED
#define CATCH_FLAG_ENUM_HPP_INCLUDED

#if !defined( __has_cpp_attribute )
#    define CATCH_ATTR_FLAG_ENUM
#elif __has_cpp_attribute( clang::flag_enum )
#    define CATCH_ATTR_FLAG_ENUM [[clang::flag_enum]]
#elif __has_cpp_attribute( gnu::flag_enum )
#    define CATCH_ATTR_FLAG_ENUM [[gnu::flag_enum]]
#else
#    define CATCH_ATTR_FLAG_ENUM
#endif

#endif // CATCH_FLAG_ENUM_HPP_INCLUDED
