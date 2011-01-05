/*
 *  catch_registry.hpp
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

#include "catch_testcase.hpp"
#include "catch_common.h"

#include <vector>
#include <set>
#include <stdexcept>

#include <iostream>

namespace Catch
{
class TestRegistry
{
public:
    
    static TestRegistry& instance()
    {
        static TestRegistry reg;
        return reg;
    }
    
    void registerTest( const TestCaseInfo& testInfo )
    {
        if( m_functions.find( testInfo ) == m_functions.end() )
        {
            m_functions.insert( testInfo );
            m_functionsInOrder.push_back( testInfo );
        }
    }
        
    const std::vector<TestCaseInfo>& getAllTests() const
    {
        return m_functionsInOrder;
    }
        
private:
        
    std::set<TestCaseInfo> m_functions;
    std::vector<TestCaseInfo> m_functionsInOrder;
};

typedef void(*TestFunction)();
    
struct FreeFunctionTestCase : TestCase
{
    FreeFunctionTestCase( TestFunction fun )
    : fun( fun )
    {}
    
    virtual void invoke() const
    {
        fun();
    }
    
    virtual TestCase* clone() const
    {
        return new FreeFunctionTestCase( fun );
    }

    virtual bool operator == ( const TestCase& other ) const
    {
        const FreeFunctionTestCase* ffOther = dynamic_cast<const FreeFunctionTestCase*> ( &other );
        return ffOther && fun == ffOther->fun;
    }
    
    virtual bool operator < ( const TestCase& other ) const
    {
        const FreeFunctionTestCase* ffOther = dynamic_cast<const FreeFunctionTestCase*> ( &other );
        return ffOther && fun < ffOther->fun;
    }
    
private:
    TestFunction fun;
};

template<typename C>
struct MethodTestCase : TestCase
{
    MethodTestCase( void (C::*method)() )
    : method( method )
    {}
    
    virtual void invoke() const
    {
        C obj;
        (obj.*method)();
    }
    
    virtual TestCase* clone() const
    {
        return new MethodTestCase<C>( method );
    }

    virtual bool operator == ( const TestCase& other ) const
    {
        const MethodTestCase* mtOther = dynamic_cast<const MethodTestCase*>( &other );
        return mtOther && method == mtOther->method;
    }
    
    virtual bool operator < ( const TestCase& other ) const
    {
        const MethodTestCase* mtOther = dynamic_cast<const MethodTestCase*>( &other );
        return mtOther && &method < &mtOther->method;
    }
    
private:
    void (C::*method)();
};
    
struct AutoReg
{
    AutoReg( TestFunction function, const std::string& name, const std::string& description )
    {
        TestRegistry::instance().registerTest( TestCaseInfo( new FreeFunctionTestCase( function ), name, description ) );
    }
    
    template<typename C>
    AutoReg( void (C::*method)(), const std::string& name, const std::string& description )
    {
        TestRegistry::instance().registerTest( TestCaseInfo( new MethodTestCase<C>( method ), name, description ) );
    }
};

template<typename T, size_t>
struct FixtureWrapper{};
    
} // end namespace Catch

#define INTERNAL_CATCH_TESTCASE( Name, Desc ) \
    static void INTERNAL_CATCH_UNIQUE_NAME( catch_internal_TestFunction )(); \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_TestFunction ), Name, Desc ); }\
    static void INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_TestFunction )()

#define CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, Name, Desc ); }

#define TEST_CASE_METHOD( ClassName, TestName, Desc )\
    namespace Catch{ template<> struct FixtureWrapper<ClassName, __LINE__> : ClassName \
    { \
        void test(); \
    }; }\
    namespace { Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &Catch::FixtureWrapper<ClassName, __LINE__>::test, TestName, Desc ); } \
    void Catch::FixtureWrapper<ClassName, __LINE__>::test()

#endif // TWOBLUECUBES_CATCH_REGISTRY_HPP_INCLUDED
