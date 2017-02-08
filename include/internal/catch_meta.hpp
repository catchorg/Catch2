/*
 *  Created by Phil on 08/02/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_META_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_META_HPP_INCLUDED

#if defined(CATCH_CONFIG_CPP11_TYPE_TRAITS)
#include <type_traits>
#endif


namespace Catch {
    
#if defined(CATCH_CONFIG_CPP11_TYPE_TRAITS)

     template <class T>
     using add_lvalue_reference = std::add_lvalue_reference<T>;

#else
    
    template <typename T>
    struct add_l_value_reference {
        typedef T& type;
    };
    template <typename T>
    struct add_l_value_reference<T&> {
        typedef T& type;
    };
    template <typename T>
    struct add_l_value_reference<T&&> {
        typedef T& type;
    };
    
#endif
   
}

#endif // TWOBLUECUBES_CATCH_META_HPP_INCLUDED