/*
 *  Created by Martin on 08/02/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TYPE_TRAITS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TYPE_TRAITS_HPP_INCLUDED

#if defined(CATCH_CONFIG_CPP11_TYPE_TRAITS)
#include <type_traits>
#endif


namespace Catch {

#if defined(CATCH_CONFIG_CPP11_TYPE_TRAITS)

     template <typename T>
     using add_lvalue_reference = std::add_lvalue_reference<T>;

     template <typename T>
     using add_const = std::add_const<T>;

#else

    template <typename T>
    struct add_const { 
        typedef const T type;
    };

    template <typename T>
    struct add_lvalue_reference {
        typedef T& type;
    };
    template <typename T>
    struct add_lvalue_reference<T&> {
        typedef T& type;
    };
    // No && overload, because that is C++11, in which case we have
    // proper type_traits implementation from the standard library

#endif

}

#endif // TWOBLUECUBES_CATCH_TYPE_TRAITS_HPP_INCLUDED
