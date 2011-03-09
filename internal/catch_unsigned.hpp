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
    enum Operator
    {
        IsEqualTo,
        IsNotEqualTo,
        IsLessThan,
        IsGreaterThan,
        IsLessThanOrEqualTo,
        IsGreaterThanOrEqualTo
    };
    
    template<typename T1, typename T2, Operator Op>
    class Evaluator{};
    
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsEqualTo>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs == rhs;
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsNotEqualTo>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs != rhs;
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThan>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs < rhs;
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThan>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs > rhs;
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThanOrEqualTo>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs >= rhs;
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThanOrEqualTo>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs <= rhs;
        }
    };
    
    template<Operator Op, typename T1, typename T2>
    bool compare( const T1& lhs, const T2& rhs )
    {
        return Evaluator<T1, T2, Op>::evaluate( lhs, rhs );
    }
    
    // unsigned X to int
    template<Operator Op> bool compare( unsigned int lhs, int rhs )
    {
        return Evaluator<unsigned int, unsigned int, Op>::evaluate( lhs, static_cast<unsigned int>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned long lhs, int rhs )
    {
        return Evaluator<unsigned long, unsigned int, Op>::evaluate( lhs, static_cast<unsigned int>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned char lhs, int rhs )
    {
        return Evaluator<unsigned char, unsigned int, Op>::evaluate( lhs, static_cast<unsigned int>( rhs ) );
    }
    
    // unsigned X to long
    template<Operator Op> bool compare( unsigned int lhs, long rhs )
    {
        return Evaluator<unsigned int, unsigned long, Op>::evaluate( lhs, static_cast<unsigned long>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned long lhs, long rhs )
    {
        return Evaluator<unsigned long, unsigned long, Op>::evaluate( lhs, static_cast<unsigned long>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned char lhs, long rhs )
    {
        return Evaluator<unsigned char, unsigned long, Op>::evaluate( lhs, static_cast<unsigned long>( rhs ) );
    }
    
    // int to unsigned X
    template<Operator Op> bool compare( int lhs, unsigned int rhs )
    {
        return Evaluator<unsigned int, unsigned int, Op>::evaluate( static_cast<unsigned int>( lhs ), rhs );
    }
    template<Operator Op> bool compare( int lhs, unsigned long rhs )
    {
        return Evaluator<unsigned int, unsigned long, Op>::evaluate( static_cast<unsigned int>( lhs ), rhs );
    }
    template<Operator Op> bool compare( int lhs, unsigned char rhs )
    {
        return Evaluator<unsigned int, unsigned char, Op>::evaluate( static_cast<unsigned int>( lhs ), rhs );
    }
    
    // long to unsigned X
    template<Operator Op> bool compare( long lhs, unsigned int rhs )
    {
        return Evaluator<unsigned long, unsigned int, Op>::evaluate( static_cast<unsigned long>( lhs ), rhs );
    }
    template<Operator Op> bool compare( long lhs, unsigned long rhs )
    {
        return Evaluator<unsigned long, unsigned long, Op>::evaluate( static_cast<unsigned long>( lhs ), rhs );
    }
    template<Operator Op> bool compare( long lhs, unsigned char rhs )
    {
        return Evaluator<unsigned long, unsigned char, Op>::evaluate( static_cast<unsigned long>( lhs ), rhs );
    }
}

#endif // TWOBLUECUBES_CATCH_UNSIGNED_HPP_INCLUDED
