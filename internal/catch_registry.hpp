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

#include "catch_testcaseinfo.hpp"
#include "catch_common.h"

#include <vector>
#include <stdexcept>

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
        m_functions.push_back( testInfo );
    }
        
    std::vector<TestCaseInfo> getAllTests() const
    {
        return m_functions;
    }
        
private:
        
    std::vector<TestCaseInfo> m_functions;
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
    
} // end namespace Catch

#define CATCH_TEST_CASE( Name, Desc ) \
    static void _CATCH_UNIQUE_NAME( __catchTestFunction )(); \
    namespace{ Catch::AutoReg _CATCH_UNIQUE_NAME( autoRegistrar )( &_CATCH_UNIQUE_NAME( __catchTestFunction ), Name, Desc ); }\
    static void _CATCH_UNIQUE_NAME( __catchTestFunction )()

#define CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
    namespace{ Catch::AutoReg _CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, Name, Desc ); }

#endif // TWOBLUECUBES_CATCH_REGISTRY_HPP_INCLUDED
