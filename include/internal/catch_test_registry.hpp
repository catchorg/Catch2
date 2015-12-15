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

void registerTestCase
    (   ITestCase* testCase,
        char const* className,
        NameAndDesc const& nameAndDesc,
        SourceLineInfo const& lineInfo );

struct AutoReg {

    AutoReg
        (   TestFunction function,
            SourceLineInfo const& lineInfo,
            NameAndDesc const& nameAndDesc );

    template<typename C>
    AutoReg
        (   void (C::*method)(),
            char const* className,
            NameAndDesc const& nameAndDesc,
            SourceLineInfo const& lineInfo ) {

        registerTestCase
            (   new MethodTestCase<C>( method ),
                className,
                nameAndDesc,
                lineInfo );
    }

    ~AutoReg();

private:
    AutoReg( AutoReg const& );
    void operator= ( AutoReg const& );
};

void registerTestCaseFunction
    (   TestFunction function,
        SourceLineInfo const& lineInfo,
        NameAndDesc const& nameAndDesc );

} // end namespace Catch

#ifdef CATCH_CONFIG_VARIADIC_MACROS
    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TESTCASE2( TestName, ... ) \
        static void TestName(); \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &TestName, CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc( __VA_ARGS__ ) ); }\
        static void TestName()
    #define INTERNAL_CATCH_TESTCASE( ... ) \
        INTERNAL_CATCH_TESTCASE2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ), __VA_ARGS__ )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, ... ) \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, "&" #QualifiedMethod, Catch::NameAndDesc( __VA_ARGS__ ), CATCH_INTERNAL_LINEINFO ); }

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEST_CASE_METHOD2( TestName, ClassName, ... )\
        namespace{ \
            struct TestName : ClassName{ \
                void test(); \
            }; \
            Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &TestName::test, #ClassName, Catch::NameAndDesc( __VA_ARGS__ ), CATCH_INTERNAL_LINEINFO ); \
        } \
        void TestName::test()
    #define INTERNAL_CATCH_TEST_CASE_METHOD( ClassName, ... ) \
        INTERNAL_CATCH_TEST_CASE_METHOD2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ), ClassName, __VA_ARGS__ )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_REGISTER_TESTCASE( Function, ... ) \
        Catch::AutoReg( Function, CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc( __VA_ARGS__ ) );

#else
    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TESTCASE2( TestName, Name, Desc ) \
        static void TestName(); \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &TestName, CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc( Name, Desc ) ); }\
        static void TestName()
    #define INTERNAL_CATCH_TESTCASE( Name, Desc ) \
        INTERNAL_CATCH_TESTCASE2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ), Name, Desc )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, "&" #QualifiedMethod, Catch::NameAndDesc( Name, Desc ), CATCH_INTERNAL_LINEINFO ); }

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEST_CASE_METHOD2( TestCaseName, ClassName, TestName, Desc )\
        namespace{ \
            struct TestCaseName : ClassName{ \
                void test(); \
            }; \
            Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &TestCaseName::test, #ClassName, Catch::NameAndDesc( TestName, Desc ), CATCH_INTERNAL_LINEINFO ); \
        } \
        void TestCaseName::test()
    #define INTERNAL_CATCH_TEST_CASE_METHOD( ClassName, TestName, Desc )\
        INTERNAL_CATCH_TEST_CASE_METHOD2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ), ClassName, TestName, Desc )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_REGISTER_TESTCASE( Function, Name, Desc ) \
        Catch::AutoReg( Function, CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc( Name, Desc ) );
#endif

#endif // TWOBLUECUBES_CATCH_TEST_REGISTRY_HPP_INCLUDED
