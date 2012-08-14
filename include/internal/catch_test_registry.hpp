/*
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REGISTRY_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REGISTRY_HPP_INCLUDED

#include "catch_common.h"
#include "catch_interfaces_testcase.h"

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
    
struct AutoReg {

    AutoReg(    TestFunction function, 
                const char* name, 
                const char* description,
                const SourceLineInfo& lineInfo );
    
    template<typename C>
    AutoReg(    void (C::*method)(), 
                const char* name, 
                const char* description,
                const SourceLineInfo& lineInfo ) {
        registerTestCase( new MethodTestCase<C>( method ), name, description, lineInfo );
    }
    
    void registerTestCase(  ITestCase* testCase, 
                            const char* name, 
                            const char* description,
                            const SourceLineInfo& lineInfo );
    
    ~AutoReg();
    
private:
    AutoReg( const AutoReg& );    
    void operator= ( const AutoReg& );
};
    
} // end namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TESTCASE( Name, Desc ) \
    static void INTERNAL_CATCH_UNIQUE_NAME( TestCaseFunction_catch_internal_ )(); \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ ), Name, Desc, CATCH_INTERNAL_LINEINFO ); }\
    static void INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ )()

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TESTCASE_NORETURN( Name, Desc ) \
    static void INTERNAL_CATCH_UNIQUE_NAME( TestCaseFunction_catch_internal_ )() CATCH_ATTRIBUTE_NORETURN; \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ ), Name, Desc, CATCH_INTERNAL_LINEINFO ); }\
    static void INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ )()

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, Name, Desc, CATCH_INTERNAL_LINEINFO ); }

///////////////////////////////////////////////////////////////////////////////
#define TEST_CASE_METHOD( ClassName, TestName, Desc )\
    namespace{ \
        struct INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ ) : ClassName{ \
            void test(); \
        }; \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::test, TestName, Desc, CATCH_INTERNAL_LINEINFO ); \
    } \
    void INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::test()

#endif // TWOBLUECUBES_CATCH_REGISTRY_HPP_INCLUDED
