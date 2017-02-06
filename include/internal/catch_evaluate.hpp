/*
 *  Created by Phil on 04/03/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4389) // '==' : signed/unsigned mismatch
#endif

#include <cstddef>

namespace Catch {
namespace Internal {

    enum Operator {
        IsEqualTo,
        IsNotEqualTo,
        IsLessThan,
        IsGreaterThan,
        IsLessThanOrEqualTo,
        IsGreaterThanOrEqualTo
    };

    template<Operator Op> struct OperatorTraits             { static const char* getName(){ return "*error*"; } };
    template<> struct OperatorTraits<IsEqualTo>             { static const char* getName(){ return "=="; } };
    template<> struct OperatorTraits<IsNotEqualTo>          { static const char* getName(){ return "!="; } };
    template<> struct OperatorTraits<IsLessThan>            { static const char* getName(){ return "<"; } };
    template<> struct OperatorTraits<IsGreaterThan>         { static const char* getName(){ return ">"; } };
    template<> struct OperatorTraits<IsLessThanOrEqualTo>   { static const char* getName(){ return "<="; } };
    template<> struct OperatorTraits<IsGreaterThanOrEqualTo>{ static const char* getName(){ return ">="; } };

    template<typename T>
    inline T& opCast(T const& t) { return const_cast<T&>(t); }

// nullptr_t support based on pull request #154 from Konstantin Baumann
#ifdef CATCH_CONFIG_CPP11_NULLPTR
    inline std::nullptr_t opCast(std::nullptr_t) { return nullptr; }
#endif // CATCH_CONFIG_CPP11_NULLPTR


    // So the compare overloads can be operator agnostic we convey the operator as a template
    // enum, which is used to specialise an Evaluator for doing the comparison.
    template<Operator Op>
    class Evaluator{};

    template<>
    struct Evaluator<IsEqualTo> {
        template<typename T1, typename T2>
        static bool evaluate( T1& lhs, T2& rhs) {
            return static_cast<bool>( lhs ==  rhs );
        }
    };
    template<>
    struct Evaluator<IsNotEqualTo> {
        template<typename T1, typename T2>
        static bool evaluate( T1& lhs, T2& rhs ) {
            return bool( lhs != rhs );
        }
    };
    template<>
    struct Evaluator<IsLessThan> {
        template<typename T1, typename T2>
        static bool evaluate( T1& lhs, T2& rhs ) {
            return bool( lhs < rhs );
        }
    };
    template<>
    struct Evaluator<IsGreaterThan> {
        template<typename T1, typename T2>
        static bool evaluate( T1& lhs, T2& rhs ) {
            return bool( lhs > rhs );
        }
    };
    template<>
    struct Evaluator<IsGreaterThanOrEqualTo> {
        template<typename T1, typename T2>
        static bool evaluate( T1& lhs, T2& rhs ) {
            return bool( lhs >= rhs );
        }
    };
    template<>
    struct Evaluator<IsLessThanOrEqualTo> {
        template<typename T1, typename T2>
        static bool evaluate( T1& lhs, T2& rhs ) {
            return bool( lhs <= rhs );
        }
    };

    // This level of indirection allows us to specialise for integer types
    // to avoid signed/ unsigned warnings

    // "base" overload
    template<Operator Op, typename T1, typename T2>
    bool compare( T1 const& lhs, T2 const& rhs ) {
        return Evaluator<Op>::evaluate( opCast( lhs ), opCast( rhs ) );
    }

    // pointer to long (when comparing against NULL)
    template<Operator Op, typename T> bool compare( long lhs, T* rhs ) {
        return Evaluator<Op>::evaluate( opCast( reinterpret_cast<T*>( lhs ) ), opCast( rhs ) );
    }
    template<Operator Op, typename T> bool compare( T* lhs, long rhs ) {
        return Evaluator<Op>::evaluate( opCast( lhs ), opCast( reinterpret_cast<T*>( rhs ) ) );
    }

    // pointer to int (when comparing against NULL)
    template<Operator Op, typename T> bool compare( int lhs, T* rhs ) {
        return Evaluator<Op>::evaluate( opCast( reinterpret_cast<T*>( lhs ) ), opCast( rhs ) );
    }
    template<Operator Op, typename T> bool compare( T* lhs, int rhs ) {
        return Evaluator<Op>::evaluate( opCast( lhs ), opCast( reinterpret_cast<T*>( rhs ) ) );
    }

// Needed?
#ifdef CATCH_CONFIG_CPP11_NULLPTR
//    // pointer to nullptr_t (when comparing against nullptr)
//    template<Operator Op, typename T> bool compare( std::nullptr_t, T* rhs ) {
//        return Evaluator<T*, T*, Op>::evaluate( nullptr, rhs );
//    }
//    template<Operator Op, typename T> bool compare( T* lhs, std::nullptr_t ) {
//        return Evaluator<T*, T*, Op>::evaluate( lhs, nullptr );
//    }
#endif // CATCH_CONFIG_CPP11_NULLPTR

} // end of namespace Internal
} // end of namespace Catch

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED
