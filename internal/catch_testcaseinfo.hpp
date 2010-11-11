/*
 *  catch_testcaseinfo.hpp
 *  Catch
 *
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED

#include <map>
#include <string>

namespace Catch
{
    struct TestCase
    {
        virtual ~TestCase(){}
        virtual void invoke() const = 0;
        virtual TestCase* clone() const = 0;
    };
    
    class TestCaseInfo
    {
    public:
        TestCaseInfo( TestCase* testCase, const std::string& name, const std::string& description )
        :   test( testCase ),
            name( name ),
            description( description )
        {
        }
        TestCaseInfo()
        :   test( NULL )
        {
        }
        
        TestCaseInfo( const TestCaseInfo& other )
        :   test( other.test->clone() ),
            name( other.name ),
            description( other.description )
        {
        }
        
        TestCaseInfo& operator = ( const TestCaseInfo& other )
        {
            TestCaseInfo temp( other );
            swap( temp );
            return *this;
        }
        
        ~TestCaseInfo()
        {
            delete test;
        }
        
        void invoke() const
        {
            test->invoke();
        }
        
        const std::string& getName() const
        {
            return name;
        }
        const std::string& getDescription() const
        {
            return description;
        }
        
        void swap( TestCaseInfo& other )
        {
            std::swap( test, other.test );
            name.swap( other.name );
            description.swap( other.description );
        }
        
    private:
        TestCase* test;
        std::string name;
        std::string description;
    };
    
}

#endif // TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED