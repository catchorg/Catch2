/*
 *  catch_objc.hpp
 *  Test
 *
 *  Created by Phil on 14/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_OBJC_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_OBJC_HPP_INCLUDED

#import <objc/runtime.h>
#include <string>
#include "catch.hpp"

namespace Catch 
{
    template<typename T>
    class OcMethod : public TestCase
    {
    public:
        OcMethod( SEL sel ) : m_sel( sel )
        {
        }
        
        virtual void invoke() const
        {
            T* obj = [[T alloc] init];
            if( [obj respondsToSelector: m_sel] )
                [obj performSelector: m_sel];
            [obj release];
        }
        
        virtual TestCase* clone() const
        {
            return new OcMethod<T>( m_sel );
        }
        
        virtual bool operator == ( const TestCase& other ) const
        {
            const OcMethod* ocmOther = dynamic_cast<const OcMethod*> ( &other );
            return ocmOther && ocmOther->m_sel == m_sel;
        }
        
        virtual bool operator < ( const TestCase& other ) const
        {
            const OcMethod* ocmOther = dynamic_cast<const OcMethod*> ( &other );
            return ocmOther && ocmOther->m_sel < m_sel;
        }
        
    private:
        
        SEL m_sel;
    };
    
    template<typename T>
    struct OcAutoReg
    {
        OcAutoReg()
        {
            u_int count;
            Method* methods = class_copyMethodList([T class], &count);        
            
            for( int i = 0; i < count ; i++ )
            {
                SEL selector = method_getName(methods[i]);
                std::string methodName = sel_getName(selector);
                if( startsWith( methodName, "Catch_TestCase_" ) )
                {
                    std::string testCaseName = methodName.substr( 15 );
                    std::string name = getAnnotation( "Name", testCaseName );
                    std::string desc = getAnnotation( "Description", testCaseName );
                    
                    TestRegistry::instance().registerTest( TestCaseInfo( new OcMethod<T>( selector ), name, desc ) );
                    
                }
            }
            free(methods);
        }
        
    private:
        bool startsWith( const std::string& str, const std::string& sub )
        {
            return str.length() > sub.length() && str.substr( 0, sub.length() ) == sub;
        }
        
        const char* getAnnotation( const std::string& annotationName, const std::string& testCaseName )
        {
            NSString* selStr = [[NSString alloc] initWithFormat:@"Catch_%s_%s", annotationName.c_str(), testCaseName.c_str()];
            SEL sel = NSSelectorFromString( selStr );
            [selStr release];
            if( [[T class] respondsToSelector: sel] )
                return (const char*)[[T class] performSelector: sel];
            return "";
        }        
        
    };    
}

#define CATCH_REGISTER_CLASS( className ) namespace{ Catch::OcAutoReg<className> reg; }

#define OC_TEST_CASE( name, desc )\
+(const char*) INTERNAL_CATCH_UNIQUE_NAME( Catch_Name_test ) \
{\
return name; \
}\
+(const char*) INTERNAL_CATCH_UNIQUE_NAME( Catch_Description_test ) \
{ \
return desc; \
} \
-(void) INTERNAL_CATCH_UNIQUE_NAME( Catch_TestCase_test )

#endif // TWOBLUECUBES_CATCH_OBJC_HPP_INCLUDED