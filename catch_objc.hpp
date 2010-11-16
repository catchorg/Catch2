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

@protocol OcFixture

@optional

-(void) setUp;
-(void) tearDown;

@end

namespace Catch 
{
    class OcMethod : public TestCase
    {
    public:
        OcMethod( Class cls, SEL sel ) : m_cls( cls ), m_sel( sel )
        {
        }
        
        virtual void invoke() const
        {
            id obj = class_createInstance( m_cls, 0 );
            obj = [obj init];
            
            if( [obj respondsToSelector: @selector(setUp) ] )
                [obj performSelector: @selector(setUp)];

            if( [obj respondsToSelector: m_sel] )
                [obj performSelector: m_sel];

            if( [obj respondsToSelector: @selector(tearDown) ] )
                [obj performSelector: @selector(tearDown)];
            
            [obj release];
        }
        
        virtual TestCase* clone() const
        {
            return new OcMethod( m_cls, m_sel );
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
        Class m_cls;
        SEL m_sel;
    };
    
    namespace Detail
    {
    
        inline bool startsWith( const std::string& str, const std::string& sub )
        {
            return str.length() > sub.length() && str.substr( 0, sub.length() ) == sub;
        }
        
        inline const char* getAnnotation( Class cls, const std::string& annotationName, const std::string& testCaseName )
        {
            NSString* selStr = [[NSString alloc] initWithFormat:@"Catch_%s_%s", annotationName.c_str(), testCaseName.c_str()];
            SEL sel = NSSelectorFromString( selStr );
            [selStr release];
            if( [cls respondsToSelector: sel] )
                return (const char*)[cls performSelector: sel];
            return "";
        }        
    }
    
    inline size_t registerTestMethods()
    {
        size_t noTestMethods = 0;
        int noClasses = objc_getClassList( NULL, 0 );
        
        std::vector<Class> classes( noClasses );
        objc_getClassList( &classes[0], noClasses );
        
        for( int c = 0; c < noClasses; c++ )
        {
            Class cls = classes[c];
            {
                u_int count;
                Method* methods = class_copyMethodList( cls, &count );
                for( int m = 0; m < count ; m++ )
                {
                    SEL selector = method_getName(methods[m]);
                    std::string methodName = sel_getName(selector);
                    if( Detail::startsWith( methodName, "Catch_TestCase_" ) )
                    {
                        std::string testCaseName = methodName.substr( 15 );
                        std::string name = Detail::getAnnotation( cls, "Name", testCaseName );
                        std::string desc = Detail::getAnnotation( cls, "Description", testCaseName );
                        
                        TestRegistry::instance().registerTest( TestCaseInfo( new OcMethod( cls, selector ), name, desc ) );
                        noTestMethods++;
                        
                    }
                }
                free(methods);              
            }
        }
        return noTestMethods;
    }  
}

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