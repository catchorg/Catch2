/*
 *  catch_test_registry.hpp
 *  Catch
 *
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_REGISTRY_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REGISTRY_HPP_INCLUDED

#include "catch_common.h"
#include "catch_interfaces_testcase.h"

namespace Catch
{
    
template<typename C>
struct MethodTestCase : ITestCase
{
    ///////////////////////////////////////////////////////////////////////////
    MethodTestCase
    (
        void (C::*method)() 
    )
    : m_method( method )
    {}
    
    ///////////////////////////////////////////////////////////////////////////
    virtual void invoke
    ()
    const
    {
        C obj;
        (obj.*m_method)();
    }
    
    ///////////////////////////////////////////////////////////////////////////
    virtual ITestCase* clone
    ()
    const
    {
        return new MethodTestCase<C>( m_method );
    }

    ///////////////////////////////////////////////////////////////////////////
    virtual bool operator == 
    (
        const ITestCase& other
    )
    const
    {
        const MethodTestCase* mtOther = dynamic_cast<const MethodTestCase*>( &other );
        return mtOther && m_method == mtOther->m_method;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    virtual bool operator < 
    (
        const ITestCase& other
    )
    const
    {
        const MethodTestCase* mtOther = dynamic_cast<const MethodTestCase*>( &other );
        return mtOther && &m_method < &mtOther->m_method;
    }
    
private:
    void (C::*m_method)();
};

typedef void(*TestFunction)();
    
struct AutoReg
{
    AutoReg
        (   TestFunction function, 
            const char* name, 
            const char* description,
            const char* filename,
            std::size_t line
        );
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename C>
    AutoReg
    (
        void (C::*method)(), 
        const char* name, 
        const char* description,
        const char* filename,
        std::size_t line
    )
    {
        registerTestCase( new MethodTestCase<C>( method ), name, description, filename, line );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    void registerTestCase
    (
        ITestCase* testCase, 
        const char* name, 
        const char* description,
        const char* filename,
        std::size_t line
    );
    
    ~AutoReg
        ();
    
private:
    AutoReg
        ( const AutoReg& );
    
    void operator=
        ( const AutoReg& );
};
    
} // end namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TESTCASE( Name, Desc ) \
    static void INTERNAL_CATCH_UNIQUE_NAME( catch_internal_TestFunction )(); \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_TestFunction ), Name, Desc, __FILE__, __LINE__ ); }\
    static void INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_TestFunction )()

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TESTCASE_NORETURN( Name, Desc ) \
    static void INTERNAL_CATCH_UNIQUE_NAME( catch_internal_TestFunction )() ATTRIBUTE_NORETURN; \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_TestFunction ), Name, Desc, __FILE__, __LINE__ ); }\
    static void INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_TestFunction )()

///////////////////////////////////////////////////////////////////////////////
#define CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, Name, Desc, __FILE__, __LINE__ ); }

///////////////////////////////////////////////////////////////////////////////
#define TEST_CASE_METHOD( ClassName, TestName, Desc )\
    namespace{ \
        struct INTERNAL_CATCH_UNIQUE_NAME( Catch_FixtureWrapper ) : ClassName{ \
            void test(); \
        }; \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &INTERNAL_CATCH_UNIQUE_NAME( Catch_FixtureWrapper )::test, TestName, Desc, __FILE__, __LINE__ ); \
    } \
    void INTERNAL_CATCH_UNIQUE_NAME( Catch_FixtureWrapper )::test()

#endif // TWOBLUECUBES_CATCH_REGISTRY_HPP_INCLUDED
