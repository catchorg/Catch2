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
#pragma warning(disable:4018) // more "signed/unsigned mismatch"
#pragma warning(disable:4312) // Converting int to T* using reinterpret_cast (issue on x64 platform)
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
    T& removeConst(T const &t) { return const_cast<T&>(t); }
#ifdef CATCH_CONFIG_CPP11_NULLPTR
    inline std::nullptr_t removeConst(std::nullptr_t) { return nullptr; }
#endif


    // So the compare overloads can be operator agnostic we convey the operator as a template
    // enum, which is used to specialise an Evaluator for doing the comparison.
    template<typename T1, typename T2, Operator Op>
    struct Evaluator{};

    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsEqualTo> {
        static bool evaluate( T1 const& lhs, T2 const& rhs) {
            return bool(removeConst(lhs) == removeConst(rhs) );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsNotEqualTo> {
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool(removeConst(lhs) != removeConst(rhs) );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThan> {
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool(removeConst(lhs) < removeConst(rhs) );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThan> {
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool(removeConst(lhs) > removeConst(rhs) );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThanOrEqualTo> {
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool(removeConst(lhs) >= removeConst(rhs) );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThanOrEqualTo> {
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool(removeConst(lhs) <= removeConst(rhs) );
        }
    };

    // Special case for comparing a pointer to an int (deduced for p==0)
    template<typename T>
    struct Evaluator<int const&, T* const&, IsEqualTo> {
        static bool evaluate( int lhs, T* rhs) {
            return reinterpret_cast<void const*>( lhs ) ==  rhs;
        }
    };
    template<typename T>
    struct Evaluator<T* const&, int const&, IsEqualTo> {
        static bool evaluate( T* lhs, int rhs) {
            return lhs == reinterpret_cast<void const*>( rhs );
        }
    };
    template<typename T>
    struct Evaluator<int const&, T* const&, IsNotEqualTo> {
        static bool evaluate( int lhs, T* rhs) {
            return reinterpret_cast<void const*>( lhs ) !=  rhs;
        }
    };
    template<typename T>
    struct Evaluator<T* const&, int const&, IsNotEqualTo> {
        static bool evaluate( T* lhs, int rhs) {
            return lhs != reinterpret_cast<void const*>( rhs );
        }
    };

    template<typename T>
    struct Evaluator<long const&, T* const&, IsEqualTo> {
        static bool evaluate( long lhs, T* rhs) {
            return reinterpret_cast<void const*>( lhs ) ==  rhs;
        }
    };
    template<typename T>
    struct Evaluator<T* const&, long const&, IsEqualTo> {
        static bool evaluate( T* lhs, long rhs) {
            return lhs == reinterpret_cast<void const*>( rhs );
        }
    };
    template<typename T>
    struct Evaluator<long const&, T* const&, IsNotEqualTo> {
        static bool evaluate( long lhs, T* rhs) {
            return reinterpret_cast<void const*>( lhs ) !=  rhs;
        }
    };
    template<typename T>
    struct Evaluator<T* const&, long const&, IsNotEqualTo> {
        static bool evaluate( T* lhs, long rhs) {
            return lhs != reinterpret_cast<void const*>( rhs );
        }
    };

} // end of namespace Internal
} // end of namespace Catch

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED
