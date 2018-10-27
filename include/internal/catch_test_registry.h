/*
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_REGISTRY_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_REGISTRY_HPP_INCLUDED

#include "catch_common.h"
#include "catch_interfaces_testcase.h"
#include "catch_compiler_capabilities.h"
#include "catch_stringref.h"

namespace Catch {

template<typename C>
class TestInvokerAsMethod : public ITestInvoker {
    void (C::*m_testAsMethod)();
public:
    TestInvokerAsMethod( void (C::*testAsMethod)() ) noexcept : m_testAsMethod( testAsMethod ) {}

    void invoke() const override {
        C obj;
        (obj.*m_testAsMethod)();
    }
};

auto makeTestInvoker( void(*testAsFunction)() ) noexcept -> ITestInvoker*;

template<typename C>
auto makeTestInvoker( void (C::*testAsMethod)() ) noexcept -> ITestInvoker* {
    return new(std::nothrow) TestInvokerAsMethod<C>( testAsMethod );
}

struct NameAndTags {
    NameAndTags( StringRef const& name_ = StringRef(), StringRef const& tags_ = StringRef() ) noexcept;
    StringRef name;
    StringRef tags;
};

struct AutoReg : NonCopyable {
    AutoReg( ITestInvoker* invoker, SourceLineInfo const& lineInfo, StringRef const& classOrMethod, NameAndTags const& nameAndTags ) noexcept;
    ~AutoReg();
};

} // end namespace Catch

#define INTERNAL_CATCH_EXPAND1(param) INTERNAL_CATCH_EXPAND2(param)
#define INTERNAL_CATCH_EXPAND2(...) INTERNAL_CATCH_NO## __VA_ARGS__
#define INTERNAL_CATCH_DEF(...) INTERNAL_CATCH_DEF __VA_ARGS__
#define INTERNAL_CATCH_NOINTERNAL_CATCH_DEF

#if defined(CATCH_CONFIG_DISABLE)
    #define INTERNAL_CATCH_TESTCASE_NO_REGISTRATION( TestName, ... ) \
        static void TestName()
    #define INTERNAL_CATCH_TESTCASE_METHOD_NO_REGISTRATION( TestName, ClassName, ... ) \
        namespace{                        \
            struct TestName : INTERNAL_CATCH_EXPAND1(INTERNAL_CATCH_DEF ClassName) { \
                void test();              \
            };                            \
        }                                 \
        void TestName::test()

	#define INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION( TestName, name, description, ...)\
		template<typename TestType> static void TestName();\
		INTERNAL_CATCH_TESTCASE_NO_REGISTRATION( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ) )\
		{\
			INTERNAL_CATCH_CALL_MACRO_X_TIMES_FOR_ARGS(INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION_NO_REGISTRATION, TestName, __VA_ARGS__)\
		}\
		template<typename TestType> static void TestName()

    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION( TestName, ClassName, ... ) \
        namespace{                        \
            template<typename TestType>   \
            struct TestName : INTERNAL_CATCH_EXPAND1(INTERNAL_CATCH_DEF ClassName <TestType>) { \
                void test();              \
            };                            \
        }                                 \
        template<typename TestType> void TestName::test()

#endif

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TESTCASE2( TestName, ... ) \
        static void TestName(); \
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( Catch::makeTestInvoker( &TestName ), CATCH_INTERNAL_LINEINFO, Catch::StringRef(), Catch::NameAndTags{ __VA_ARGS__ } ); } /* NOLINT */ \
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS \
        static void TestName()
    #define INTERNAL_CATCH_TESTCASE( ... ) \
        INTERNAL_CATCH_TESTCASE2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ), __VA_ARGS__ )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, ... ) \
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( Catch::makeTestInvoker( &QualifiedMethod ), CATCH_INTERNAL_LINEINFO, "&" #QualifiedMethod, Catch::NameAndTags{ __VA_ARGS__ } ); } /* NOLINT */ \
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEST_CASE_METHOD2( TestName, ClassName, ... )\
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        namespace{ \
            struct TestName : INTERNAL_CATCH_EXPAND1(INTERNAL_CATCH_DEF ClassName) { \
                void test(); \
            }; \
            Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( Catch::makeTestInvoker( &TestName::test ), CATCH_INTERNAL_LINEINFO, #ClassName, Catch::NameAndTags{ __VA_ARGS__ } ); /* NOLINT */ \
        } \
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS \
        void TestName::test()
    #define INTERNAL_CATCH_TEST_CASE_METHOD( ClassName, ... ) \
        INTERNAL_CATCH_TEST_CASE_METHOD2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ), ClassName, __VA_ARGS__ )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_REGISTER_TESTCASE( Function, ... ) \
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( Catch::makeTestInvoker( Function ), CATCH_INTERNAL_LINEINFO, Catch::StringRef(), Catch::NameAndTags{ __VA_ARGS__ } ); /* NOLINT */ \
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS

    ///////////////////////////////////////////////////////////////////////////////
    #define CATCH_GET_NTH_ARG( _1, _2, _3, _4, _5, _6, _7, _8, N, ... ) N

#ifdef CATCH_INTERNAL_TRADITIONAL_MSVC_PREPROCESSOR
    #define CATCH_INTERNAL_EXPAND_VARGS( ... ) __VA_ARGS__

    // function expanding helpers
    #define _fe_0( _call, x, ... )
    #define _fe_1( _call, x, y ) CATCH_INTERNAL_EXPAND_VARGS( _call( x, y ) );
    #define _fe_2( _call, x, y, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call( x, y ) ); CATCH_INTERNAL_EXPAND_VARGS( _fe_1( _call, x, __VA_ARGS__ ) )
    #define _fe_3( _call, x, y, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call( x, y ) ); CATCH_INTERNAL_EXPAND_VARGS( _fe_2( _call, x, __VA_ARGS__ ) )
    #define _fe_4( _call, x, y, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call( x, y ) ); CATCH_INTERNAL_EXPAND_VARGS( _fe_3( _call, x, __VA_ARGS__ ) )
    #define _fe_5( _call, x, y, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call( x, y ) ); CATCH_INTERNAL_EXPAND_VARGS( _fe_4( _call, x, __VA_ARGS__ ) )
    #define _fe_6( _call, x, y, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call( x, y ) ); CATCH_INTERNAL_EXPAND_VARGS( _fe_5( _call, x, __VA_ARGS__ ) )
    #define _fe_7( _call, x, y, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call( x, y ) ); CATCH_INTERNAL_EXPAND_VARGS( _fe_6( _call, x, __VA_ARGS__ ) )
    #define _fe_8( _call, x, y, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call( x, y ) ); CATCH_INTERNAL_EXPAND_VARGS( _fe_7( _call, x, __VA_ARGS__ ) )

    #define INTERNAL_CATCH_CALL_MACRO_X_TIMES_FOR_ARGS( macro, test, ... )\
        CATCH_INTERNAL_EXPAND_VARGS( CATCH_INTERNAL_EXPAND_VARGS( CATCH_GET_NTH_ARG( __VA_ARGS__, \
        _fe_8, _fe_7, _fe_6, _fe_5, _fe_4, _fe_3, \
        _fe_2, _fe_1, _fe_0 ) )( macro, test, __VA_ARGS__ ) )

    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE( name, description, ... ) CATCH_INTERNAL_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_TEST_CASE2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), name, description, __VA_ARGS__ ) )

    #define _fem_0( _call, w, x, y, z, ... )
    #define _fem_1( _call, v, w, x, y, z ) CATCH_INTERNAL_EXPAND_VARGS( _call( v, w, x, y, z ) );
    #define _fem_2( _call, v, w, x, y, z, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call(v, w, x, y, z ) ); CATCH_INTERNAL_EXPAND_VARGS( _fem_1( _call, v, w, x, y, __VA_ARGS__ ) )
    #define _fem_3( _call, v, w, x, y, z, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call(v, w, x, y, z ) ); CATCH_INTERNAL_EXPAND_VARGS( _fem_2( _call, v, w, x, y, __VA_ARGS__ ) )
    #define _fem_4( _call, v, w, x, y, z, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call(v, w, x, y, z ) ); CATCH_INTERNAL_EXPAND_VARGS( _fem_3( _call, v, w, x, y, __VA_ARGS__ ) )
    #define _fem_5( _call, v, w, x, y, z, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call(v, w, x, y, z ) ); CATCH_INTERNAL_EXPAND_VARGS( _fem_4( _call, v, w, x, y, __VA_ARGS__ ) )
    #define _fem_6( _call, v, w, x, y, z, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call(v, w, x, y, z ) ); CATCH_INTERNAL_EXPAND_VARGS( _fem_5( _call, v, w, x, y, __VA_ARGS__ ) )
    #define _fem_7( _call, v, w, x, y, z, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call(v, w, x, y, z ) ); CATCH_INTERNAL_EXPAND_VARGS( _fem_6( _call, v, w, x, y, __VA_ARGS__ ) )
    #define _fem_8( _call, v, w, x, y, z, ... ) CATCH_INTERNAL_EXPAND_VARGS( _call(v, w, x, y, z ) ); CATCH_INTERNAL_EXPAND_VARGS( _fem_7( _call, v, w, x, y, __VA_ARGS__ ) )

    #define INTERNAL_CATCH_CALL_MACRO_X_TIMES_FOR_ARGS_M( macro, TestName, ClassName, name, description, ... )\
        CATCH_INTERNAL_EXPAND_VARGS( CATCH_INTERNAL_EXPAND_VARGS( CATCH_GET_NTH_ARG( __VA_ARGS__, \
        _fem_8, _fem_7, _fem_6, _fem_5, _fem_4, _fem_3, \
        _fem_2, _fem_1, _fem_0 ) )( macro, TestName, ClassName, name, description, __VA_ARGS__ ) )

    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD( ClassName, ... ) \
        CATCH_INTERNAL_EXPAND_VARGS(INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), ClassName, __VA_ARGS__ ) )

#else
    // function expanding helpers
    #define _fe_0( _call, x, ... )
    #define _fe_1( _call, x, y ) _call( x, y );
    #define _fe_2( _call, x, y, ... ) _call( x, y ); _fe_1( _call, x, __VA_ARGS__ )
    #define _fe_3( _call, x, y, ... ) _call( x, y ); _fe_2( _call, x, __VA_ARGS__ )
    #define _fe_4( _call, x, y, ... ) _call( x, y ); _fe_3( _call, x, __VA_ARGS__ )
    #define _fe_5( _call, x, y, ... ) _call( x, y ); _fe_4( _call, x, __VA_ARGS__ )
    #define _fe_6( _call, x, y, ... ) _call( x, y ); _fe_5( _call, x, __VA_ARGS__ )
    #define _fe_7( _call, x, y, ... ) _call( x, y ); _fe_6( _call, x, __VA_ARGS__ )
    #define _fe_8( _call, x, y, ... ) _call( x, y ); _fe_7( _call, x, __VA_ARGS__ )

    #define INTERNAL_CATCH_CALL_MACRO_X_TIMES_FOR_ARGS( macro, test, ... )\
        CATCH_GET_NTH_ARG( __VA_ARGS__, \
        _fe_8, _fe_7, _fe_6, _fe_5, _fe_4, _fe_3, \
        _fe_2, _fe_1, _fe_0 )( macro, test, __VA_ARGS__ )

    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE( name, description, ... ) INTERNAL_CATCH_TEMPLATE_TEST_CASE2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), name, description, __VA_ARGS__ )

    #define _fem_0( _call, w, x, y, z, ... )
    #define _fem_1( _call, v, w, x, y, z ) _call( v, w, x, y, z );
    #define _fem_2( _call, v, w, x, y, z, ... ) _call(v, w, x, y, z ); _fem_1( _call, v, w, x, y, __VA_ARGS__ )
    #define _fem_3( _call, v, w, x, y, z, ... ) _call(v, w, x, y, z ); _fem_2( _call, v, w, x, y, __VA_ARGS__ )
    #define _fem_4( _call, v, w, x, y, z, ... ) _call(v, w, x, y, z ); _fem_3( _call, v, w, x, y, __VA_ARGS__ )
    #define _fem_5( _call, v, w, x, y, z, ... ) _call(v, w, x, y, z ); _fem_4( _call, v, w, x, y, __VA_ARGS__ )
    #define _fem_6( _call, v, w, x, y, z, ... ) _call(v, w, x, y, z ); _fem_5( _call, v, w, x, y, __VA_ARGS__ )
    #define _fem_7( _call, v, w, x, y, z, ... ) _call(v, w, x, y, z ); _fem_6( _call, v, w, x, y, __VA_ARGS__ )
    #define _fem_8( _call, v, w, x, y, z, ... ) _call(v, w, x, y, z ); _fem_7( _call, v, w, x, y, __VA_ARGS__ )

    #define INTERNAL_CATCH_CALL_MACRO_X_TIMES_FOR_ARGS_M( macro, TestName, ClassName, name, description, ... )\
        CATCH_GET_NTH_ARG( __VA_ARGS__, \
        _fem_8, _fem_7, _fem_6, _fem_5, _fem_4, _fem_3, \
        _fem_2, _fem_1, _fem_0 )( macro, TestName, ClassName, name, description, __VA_ARGS__ )

    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD( ClassName, ... ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), ClassName, __VA_ARGS__ )

#endif

    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE2( TestName, name, description, ... )\
        template<typename TestType> static void TestName();\
        INTERNAL_CATCH_TESTCASE( name, description )\
        {\
            INTERNAL_CATCH_CALL_MACRO_X_TIMES_FOR_ARGS( INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION, TestName, __VA_ARGS__ )\
        }\
        template<typename TestType> static void TestName()


    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_REGISTER( TestName, ClassName, name, description, Tn)\
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( Catch::makeTestInvoker( &TestName<Tn>::test ), CATCH_INTERNAL_LINEINFO, #ClassName "<" #Tn ">", Catch::NameAndTags{ name " - " #Tn, description } ) /* NOLINT */

    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD2( TestName, ClassName, name, description, ... )\
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        namespace{ \
            template<typename TestType> \
            struct TestName : INTERNAL_CATCH_EXPAND1(INTERNAL_CATCH_DEF ClassName <TestType>) { \
                void test(); \
            }; \
            INTERNAL_CATCH_CALL_MACRO_X_TIMES_FOR_ARGS_M(INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_REGISTER, TestName, ClassName, name, description, __VA_ARGS__) \
        } \
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS \
        template<typename TestType> void TestName<TestType>::test()


#endif // TWOBLUECUBES_CATCH_TEST_REGISTRY_HPP_INCLUDED
