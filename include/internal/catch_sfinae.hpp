/*
 *  Created by Phil on 15/04/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_SFINAE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_SFINAE_HPP_INCLUDED

// Try to detect if the current compiler supports SFINAE
#include "catch_compiler_capabilities.h"

#ifdef CATCH_SFINAE

namespace Catch {
    
    struct TrueType {
        static const bool value = true;
        typedef void Enable;
        char sizer[1];
    };
    struct FalseType {
        static const bool value = false;
        typedef void Disable;
        char sizer[2];
    };

    template<bool> struct NotABooleanExpression;

    template<bool c> struct If : NotABooleanExpression<c> {};
    template<> struct If<true> : TrueType {};
    template<> struct If<false> : FalseType {};

    template<int size> struct SizedIf;
    template<> struct SizedIf<sizeof(TrueType)> : TrueType {};
    template<> struct SizedIf<sizeof(FalseType)> : FalseType {};
    
} // end namespace Catch

#endif // CATCH_SFINAE

#endif // TWOBLUECUBES_CATCH_SFINAE_HPP_INCLUDED

