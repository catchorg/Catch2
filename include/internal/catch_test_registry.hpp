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
#include "internal/catch_compiler_capabilities.h"

namespace Catch {

template<typename C>
class MethodTestCase : public SharedImpl<ITestCase> {

public:
    MethodTestCase( void (C::*method)() ) : m_method( method ) {}

    virtual void invoke() const {
        C obj;
        (obj.*m_method)();
    }

private:
    virtual ~MethodTestCase() {}

    void (C::*m_method)();
};

typedef void(*TestFunction)();

struct NameAndDesc {
    NameAndDesc( const char* _name = "", const char* _description= "" )
    : name( _name ), description( _description )
    {}

    const char* name;
    const char* description;
};

struct AutoReg {

    AutoReg(    TestFunction function,
                SourceLineInfo const& lineInfo,
                NameAndDesc const& nameAndDesc );

    template<typename C>
    AutoReg(    void (C::*method)(),
                char const* className,
                NameAndDesc const& nameAndDesc,
                SourceLineInfo const& lineInfo ) {
        registerTestCase(   new MethodTestCase<C>( method ),
                            className,
                            nameAndDesc,
                            lineInfo );
    }

    void registerTestCase(  ITestCase* testCase,
                            char const* className,
                            NameAndDesc const& nameAndDesc,
                            SourceLineInfo const& lineInfo );

    ~AutoReg();

private:
    AutoReg( AutoReg const& );
    void operator= ( AutoReg const& );
};

} // end namespace Catch

#ifdef CATCH_CONFIG_VARIADIC_MACROS
    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TESTCASE( ... ) \
        static void INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ )(); \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  ____C_A_T_C_H____T_E_S_T____ ), CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc( __VA_ARGS__ ) ); }\
        static void INTERNAL_CATCH_UNIQUE_NAME(  ____C_A_T_C_H____T_E_S_T____ )()

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, ... ) \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, "&" #QualifiedMethod, Catch::NameAndDesc( __VA_ARGS__ ), CATCH_INTERNAL_LINEINFO ); }

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEST_CASE_METHOD( ClassName, ... )\
        namespace{ \
            struct INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ) : ClassName{ \
                void test(); \
            }; \
            Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ )::test, #ClassName, Catch::NameAndDesc( __VA_ARGS__ ), CATCH_INTERNAL_LINEINFO ); \
        } \
        void INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ )::test()

#else
    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TESTCASE( Name, Desc ) \
        static void INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ )(); \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  ____C_A_T_C_H____T_E_S_T____ ), CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc( Name, Desc ) ); }\
        static void INTERNAL_CATCH_UNIQUE_NAME(  ____C_A_T_C_H____T_E_S_T____ )()

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, "&" #QualifiedMethod, Catch::NameAndDesc( Name, Desc ), CATCH_INTERNAL_LINEINFO ); }

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEST_CASE_METHOD( ClassName, TestName, Desc )\
        namespace{ \
            struct INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ) : ClassName{ \
                void test(); \
            }; \
            Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ )::test, #ClassName, Catch::NameAndDesc( TestName, Desc ), CATCH_INTERNAL_LINEINFO ); \
        } \
        void INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ )::test()

#endif

///////////////////////////////////////////////////////////////////////////////
// Type-parameterised tests via test case templates
// See https://github.com/philsquared/Catch/issues/46
// and https://github.com/philsquared/Catch/issues/357

#define INTERNAL_CATCH_TEST_CASE_TEMPLATE_DECL( T ) \
    template<typename T> \
    static void INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_TE____T_E_S_T____ )();

#define INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( Tn ) \
        INTERNAL_CATCH_SECTION( #Tn, "" ) \
        { \
            INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_TE____T_E_S_T____ )<Tn>(); \
        }

#define INTERNAL_CATCH_TEST_CASE_TEMPLATE_DEFN( T ) \
    template<typename T> \
    static void INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_TE____T_E_S_T____ )()

// Simple repetition, which could be accomplished more tersely with some extra preprocessor magic

#define INTERNAL_CATCH_TEMPLATE_TEST_CASE_1( name, description, T, T1 ) \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DECL( T ) \
    INTERNAL_CATCH_TESTCASE( name, description ) \
    { \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T1 ) \
    } \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DEFN( T )

#define INTERNAL_CATCH_TEMPLATE_TEST_CASE_2( name, description, T, T1, T2 ) \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DECL( T ) \
    INTERNAL_CATCH_TESTCASE( name, description ) \
    { \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T1 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T2 ) \
    } \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DEFN( T )

#define INTERNAL_CATCH_TEMPLATE_TEST_CASE_3( name, description, T, T1, T2, T3 ) \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DECL( T ) \
    INTERNAL_CATCH_TESTCASE( name, description ) \
    { \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T1 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T2 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T3 ) \
    } \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DEFN( T )

#define INTERNAL_CATCH_TEMPLATE_TEST_CASE_4( name, description, T, T1, T2, T3, T4 ) \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DECL( T ) \
    INTERNAL_CATCH_TESTCASE( name, description ) \
    { \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T1 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T2 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T3 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T4 ) \
    } \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DEFN( T )

#define INTERNAL_CATCH_TEMPLATE_TEST_CASE_5( name, description, T, T1, T2, T3, T4, T5 ) \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DECL( T ) \
    INTERNAL_CATCH_TESTCASE( name, description ) \
    { \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T1 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T2 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T3 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T4 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T5 ) \
    } \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DEFN( T )

#define INTERNAL_CATCH_TEMPLATE_TEST_CASE_6( name, description, T, T1, T2, T3, T4, T5, T6 ) \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DECL( T ) \
    INTERNAL_CATCH_TESTCASE( name, description ) \
    { \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T1 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T2 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T3 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T4 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T5 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T6 ) \
    } \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DEFN( T )

#define INTERNAL_CATCH_TEMPLATE_TEST_CASE_7( name, description, T, T1, T2, T3, T4, T5, T6, T7 ) \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DECL( T ) \
    INTERNAL_CATCH_TESTCASE( name, description ) \
    { \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T1 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T2 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T3 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T4 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T5 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T6 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T7 ) \
    } \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DEFN( T )

#define INTERNAL_CATCH_TEMPLATE_TEST_CASE_8( name, description, T, T1, T2, T3, T4, T5, T6, T7, T8 ) \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DECL( T ) \
    INTERNAL_CATCH_TESTCASE( name, description ) \
    { \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T1 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T2 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T3 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T4 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T5 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T6 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T7 ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_SECTION( T8 ) \
    } \
    INTERNAL_CATCH_TEST_CASE_TEMPLATE_DEFN( T )

#endif // TWOBLUECUBES_CATCH_TEST_REGISTRY_HPP_INCLUDED
