/*
 *  catch_evaluate.hpp
 *  Catch
 *
 *  Created by Phil on 04/03/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED

namespace Catch
{
namespace Internal
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
    
    template<Operator Op>
    struct OperatorTraits{ static const char* getName(){ return "*error - unknown operator*"; } };

    template<>
    struct OperatorTraits<IsEqualTo>{ static const char* getName(){ return "=="; } };
    
    template<>
    struct OperatorTraits<IsNotEqualTo>{ static const char* getName(){ return "!="; } };
    
    template<>
    struct OperatorTraits<IsLessThan>{ static const char* getName(){ return "<"; } };
    
    template<>
    struct OperatorTraits<IsGreaterThan>{ static const char* getName(){ return ">"; } };
    
    template<>
    struct OperatorTraits<IsLessThanOrEqualTo>{ static const char* getName(){ return "<="; } };
    
    template<>
    struct OperatorTraits<IsGreaterThanOrEqualTo>{ static const char* getName(){ return ">="; } };
    
    // Because we capture the LHS and RHS of a binary condition expression by reference, then 
    // compare the referenced values later, we may get compiler warnings when comparing unsigned
    // integer types with integer literals (which are signed - int or long, specifically).
    // To avoid this warning we filter out the problem cases as a set of overloads of the compare
    // function. In those overloads we cast the unsigned type to its signed equivalent then
    // perform the comparison. However we also have to handle the case where the signed value is
    // negative. Comparing a negative value with an unsigned value (which will always be positive)
    // has fixed logic per operator, so this is captured seperately as an enum value.
    enum LostSign
    {
        None = 0,
        LhsSignWasLost = 1,
        RhsSignWasLost = 2
    };
    
    // So the compare overloads can be operator agnostic we convey the operator as a template
    // enum, which is used to specialise an Evaluator for doing the comparison.
    template<typename T1, typename T2, Operator Op>
    class Evaluator{};
    
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsEqualTo>
    {
        enum{ failsWhen = LhsSignWasLost | RhsSignWasLost };
        
        static bool evaluate( const T1& lhs, const T2& rhs)
        {
            return lhs == rhs;
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsNotEqualTo>
    {
        enum{ failsWhen = None };

        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs != rhs;
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThan>
    {
        enum{ failsWhen = RhsSignWasLost };
        
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs < rhs;
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThan>
    {
        enum{ failsWhen = LhsSignWasLost };

        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs > rhs;
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThanOrEqualTo>
    {
        enum{ failsWhen = LhsSignWasLost };
        
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs >= rhs;
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThanOrEqualTo>
    {
        enum{ failsWhen = RhsSignWasLost };
        
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return lhs <= rhs;
        }
    };
    
    // All the special case signed/ unsigned overloads of compare forward to this function, 
    // which, for negative numbers checks the special case fixed logic, otherwise forwards on
    // to the specialised Evaluator for the operator enum
    template<Operator Op, typename T1, typename T2>
    bool applyEvaluator( const T1& lhs, const T2& rhs, LostSign lostSign )
    {
        typedef Evaluator<T1, T2, Op> EvaluatorType;
        return lostSign == None
            ? EvaluatorType::evaluate( lhs, rhs )
            : ( EvaluatorType::failsWhen & lostSign ) != lostSign;
    }
    
    
    template<typename T>
    LostSign testLhsSign( T lhs )
    {
        return lhs < 0 ? LhsSignWasLost : None;
    }

    template<typename T>
    LostSign testRhsSign( T rhs )
    {
        return rhs < 0 ? RhsSignWasLost : None;
    }
    
    // "base" overload
    template<Operator Op, typename T1, typename T2>
    bool compare( const T1& lhs, const T2& rhs )
    {
        return Evaluator<T1, T2, Op>::evaluate( lhs, rhs );
    }
    
    // unsigned X to int
    template<Operator Op> bool compare( unsigned int lhs, int rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ), testRhsSign( rhs ) );
    }
    template<Operator Op> bool compare( unsigned long lhs, int rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ), testRhsSign( rhs )  );
    }
    template<Operator Op> bool compare( unsigned char lhs, int rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ), testRhsSign( rhs )  );
    }
    
    // unsigned X to long
    template<Operator Op> bool compare( unsigned int lhs, long rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ), testRhsSign( rhs )  );
    }
    template<Operator Op> bool compare( unsigned long lhs, long rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ), testRhsSign( rhs )  );
    }
    template<Operator Op> bool compare( unsigned char lhs, long rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ), testRhsSign( rhs )  );
    }
    
    // int to unsigned X
    template<Operator Op> bool compare( int lhs, unsigned int rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs, testLhsSign( lhs )  );
    }
    template<Operator Op> bool compare( int lhs, unsigned long rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs, testLhsSign( lhs ) );
    }
    template<Operator Op> bool compare( int lhs, unsigned char rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs, testLhsSign( lhs ) );
    }
    
    // long to unsigned X
    template<Operator Op> bool compare( long lhs, unsigned int rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs, testLhsSign( lhs ) );
    }
    template<Operator Op> bool compare( long lhs, unsigned long rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs, testLhsSign( lhs ) );
    }
    template<Operator Op> bool compare( long lhs, unsigned char rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs, testLhsSign( lhs ) );
    }

    template<Operator Op, typename T>
    bool compare( long lhs, const T* rhs )
    {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( NULL ), rhs );

    }
    
    template<Operator Op, typename T>
    bool compare( long lhs, T* rhs )
    {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( lhs ), rhs );
        
    }
    
} // end of namespace Internal
} // end of namespace Catch

#endif // TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED
