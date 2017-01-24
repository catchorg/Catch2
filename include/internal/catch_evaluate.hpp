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
    template<typename T1, typename T2, Operator Op>
    class Evaluator{};

    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsEqualTo> {
        static bool evaluate( T1 const& lhs, T2 const& rhs) {
            return bool( opCast( lhs ) ==  opCast( rhs ) );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsNotEqualTo> {
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool( opCast( lhs ) != opCast( rhs ) );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThan> {
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool( opCast( lhs ) < opCast( rhs ) );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThan> {
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool( opCast( lhs ) > opCast( rhs ) );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThanOrEqualTo> {
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool( opCast( lhs ) >= opCast( rhs ) );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThanOrEqualTo> {
        static bool evaluate( T1 const& lhs, T2 const& rhs ) {
            return bool( opCast( lhs ) <= opCast( rhs ) );
        }
    };

    template<Operator Op, typename T1, typename T2>
    bool applyEvaluator( T1 const& lhs, T2 const& rhs ) {
        return Evaluator<T1, T2, Op>::evaluate( lhs, rhs );
    }

    // This level of indirection allows us to specialise for integer types
    // to avoid signed/ unsigned warnings

    // prevent from overloading ambiguity: SunStudio 11 can't handle function overloads, but can handle class overloads...
    template<Operator Op, typename T1, typename T2>
    struct EvaluatorImpl
    {
        bool operator()(T1 const& lhs, T2 const& rhs)
        {
            return Evaluator<T1, T2, Op>::evaluate( lhs, rhs );
        }
    };

    // "base" overload
    template<Operator Op, typename T1, typename T2>
    bool compare( T1 const& lhs, T2 const& rhs ) {
        return EvaluatorImpl<Op, T1, T2>() ( lhs, rhs );
    }

#undef CATCH_INTERNAL_COMPARE
#undef CATCH_INTERNAL_COMPARE_T
#define CATCH_INTERNAL_COMPARE( LHS, RHS )   template<Operator Op>              struct EvaluatorImpl<Op, LHS, RHS> { bool operator()(LHS lhs, RHS rhs)
#define CATCH_INTERNAL_COMPARE_T( LHS, RHS ) template<Operator Op, typename T>  struct EvaluatorImpl<Op, LHS, RHS> { bool operator()(LHS lhs, RHS rhs)

    // unsigned X to int
    CATCH_INTERNAL_COMPARE( unsigned int , int  ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ) );
    }};

    CATCH_INTERNAL_COMPARE( unsigned long , int  ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ) );
    }};
    CATCH_INTERNAL_COMPARE( unsigned char , int  ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ) );
    }};

    // unsigned X to long
    CATCH_INTERNAL_COMPARE( unsigned int , long  ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ) );
    }};
    CATCH_INTERNAL_COMPARE( unsigned long , long  ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ) );
    }};
    CATCH_INTERNAL_COMPARE( unsigned char , long  ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ) );
    }};

    // int to unsigned X
    CATCH_INTERNAL_COMPARE( int , unsigned int  ) {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE( int , unsigned long  ) {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE( int , unsigned char  ) {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs );
    }};

    // long to unsigned X
    CATCH_INTERNAL_COMPARE( long , unsigned int  ) {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE( long , unsigned long  ) {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE( long , unsigned char  ) {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs );
    }};

    // pointer to long (when comparing against NULL)
    CATCH_INTERNAL_COMPARE_T( long , T*  ) {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( lhs ), rhs );
    }};
    template<Operator Op, typename T> bool compare( T* lhs, long rhs ) {
        return Evaluator<T*, T*, Op>::evaluate( lhs, reinterpret_cast<T*>( rhs ) );
    }

    // pointer to int (when comparing against NULL)
    CATCH_INTERNAL_COMPARE_T( int , T*  ) {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE_T( T* , int  ) {
        return Evaluator<T*, T*, Op>::evaluate( lhs, reinterpret_cast<T*>( rhs ) );
    }};

#ifdef CATCH_CONFIG_CPP11_LONG_LONG
    // long long to unsigned X
    CATCH_INTERNAL_COMPARE( long long , unsigned int  ) {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE( long long , unsigned long  ) {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE( long long , unsigned long long  ) {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE( long long , unsigned char  ) {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs );
    }};

    // unsigned long long to X
    CATCH_INTERNAL_COMPARE( unsigned long long , int  ) {
        return applyEvaluator<Op>( static_cast<long>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE( unsigned long long , long  ) {
        return applyEvaluator<Op>( static_cast<long>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE( unsigned long long , long long  ) {
        return applyEvaluator<Op>( static_cast<long>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE( unsigned long long , char  ) {
        return applyEvaluator<Op>( static_cast<long>( lhs ), rhs );
    }};

    // pointer to long long (when comparing against NULL)
    CATCH_INTERNAL_COMPARE_T( long long lhs, T*  ) {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( lhs ), rhs );
    }};
    CATCH_INTERNAL_COMPARE_T( T* , long long  ) {
        return Evaluator<T*, T*, Op>::evaluate( lhs, reinterpret_cast<T*>( rhs ) );
    }};
#endif // CATCH_CONFIG_CPP11_LONG_LONG

#ifdef CATCH_CONFIG_CPP11_NULLPTR
    // pointer to nullptr_t (when comparing against nullptr)
    CATCH_INTERNAL_COMPARE_T( std::nullptr_t, T*  ) {
        return Evaluator<T*, T*, Op>::evaluate( nullptr, rhs );
    }};
    CATCH_INTERNAL_COMPARE_T( T* , std::nullptr_t ) {
        return Evaluator<T*, T*, Op>::evaluate( lhs, nullptr );
    }};
#endif // CATCH_CONFIG_CPP11_NULLPTR

#undef CATCH_INTERNAL_COMPARE
#undef CATCH_INTERNAL_COMPARE_T

} // end of namespace Internal
} // end of namespace Catch

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED
