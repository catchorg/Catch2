/*
 *  catch_unsigned.hpp
 *  Catch
 *
 *  Created by Phil on 04/03/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_UNSIGNED_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_UNSIGNED_HPP_INCLUDED

#include <limits>

namespace Catch
{
    template<typename T>
    struct SignTraits{};
    
    template<>
    struct SignTraits<int>
    {
        typedef int SignedType;
        typedef unsigned int UnsignedType;
    };
    template<>
    struct SignTraits<unsigned int> : SignTraits<int>{};
    
    template<>
    struct SignTraits<long>
    {
        typedef long SignedType;
        typedef unsigned long UnsignedType;
    };
    template<>
    struct SignTraits<unsigned long> : SignTraits<long>{};
    
    template<>
    struct SignTraits<char>
    {
        typedef char SignedType;
        typedef unsigned char UnsignedType;
    };
    template<>
    struct SignTraits<unsigned char> : SignTraits<char>{};
    
    template<typename ActualT, typename TestT>
    struct IsType
    {
        enum{ result = false };
    };
    
    template<typename T>
    struct IsType<T, T>
    {
        enum{ result = true };
    };
    
    template<typename T1, typename T2, bool>
    struct MatchedSigns
    {
        static const T1& castLhs( const T1& lhs )
        {
            return lhs;
        }
        static const T2& castRhs( const T2& rhs )
        {
            return rhs;
        }
    };
    
    template<typename T1, typename T2>
    struct MatchedSigns<T1, T2, false>
    {
        typedef typename SignTraits<T1>::SignedType Type1;
        typedef typename SignTraits<T2>::SignedType Type2;
        
        static Type1 castLhs( T1 lhs )
        {
            return static_cast<Type1>( lhs );
        }
        static Type2 castRhs( T2 rhs )
        {
            return static_cast<Type2>( rhs );
        }
    };
    
    template<typename T1, typename T2>
    struct MatchSign 
        : MatchedSigns< T1, T2, 
            (   !IsType<T1, int>::result && 
                !IsType<T1, long>::result && 
                !IsType<T2, int>::result && 
                !IsType<T2, long>::result ) ||
                !std::numeric_limits<T1>::is_integer || 
                !std::numeric_limits<T2>::is_integer ||
                std::numeric_limits<T1>::is_signed == std::numeric_limits<T2>::is_signed >
    {
    };
}

#endif // TWOBLUECUBES_CATCH_UNSIGNED_HPP_INCLUDED
