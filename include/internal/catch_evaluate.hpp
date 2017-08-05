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

    const char* operatorName(Operator op);

    template<typename T>
    T& removeConst(T const &t) { return const_cast<T&>(t); }


    // So the compare overloads can be operator agnostic we convey the operator as a template
    // enum, which is used to specialise an Evaluator for doing the comparison.
    template<Operator Op>
    struct Evaluator{};

    template<>
    struct Evaluator<IsEqualTo> {
        template<typename T1, typename T2>
        static bool evaluate( T1 const& lhs, T2 const& rhs) {
            return bool(removeConst(lhs) == removeConst(rhs) );
        }
        template<typename T>
        static bool evaluate( int lhs, T* rhs) {
            return reinterpret_cast<void const*>( lhs ) ==  rhs;
        }
        template<typename T>
        static bool evaluate( T* lhs, int rhs) {
            return lhs == reinterpret_cast<void const*>( rhs );
        }
    };
    template<>
    struct Evaluator<IsNotEqualTo> {
        template<typename T1, typename T2>
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool(removeConst(lhs) != removeConst(rhs) );
        }
        template<typename T>
        static bool evaluate( int lhs, T* rhs) {
            return reinterpret_cast<void const*>( lhs ) !=  rhs;
        }
        template<typename T>
        static bool evaluate( T* lhs, int rhs) {
            return lhs != reinterpret_cast<void const*>( rhs );
        }
    };
    template<>
    struct Evaluator<IsLessThan> {
        template<typename T1, typename T2>
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool(removeConst(lhs) < removeConst(rhs) );
        }
    };
    template<>
    struct Evaluator<IsGreaterThan> {
        template<typename T1, typename T2>
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool(removeConst(lhs) > removeConst(rhs) );
        }
    };
    template<>
    struct Evaluator<IsGreaterThanOrEqualTo> {
        template<typename T1, typename T2>
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool(removeConst(lhs) >= removeConst(rhs) );
        }
    };
    template<>
    struct Evaluator<IsLessThanOrEqualTo> {
        template<typename T1, typename T2>
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool(removeConst(lhs) <= removeConst(rhs) );
        }
    };

} // end of namespace Internal
} // end of namespace Catch

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED
