/*
 *  catch_objc.hpp
 *  Catch
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

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

#include <string>

// NB. Any general catch headers included here must be included
// in catch.hpp first to make sure they are included by the single
// header for non obj-usage
#include "internal/catch_test_case_info.hpp"

///////////////////////////////////////////////////////////////////////////////
// This protocol is really only here for (self) documenting purposes, since
// all its methods are optional.
@protocol OcFixture

@optional

-(void) setUp;
-(void) tearDown;

@end

namespace Catch 
{
    class OcMethod : public ITestCase
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        OcMethod
        (
            Class cls, 
            SEL sel
        )
        :   m_cls( cls ), 
            m_sel( sel )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////
        virtual void invoke
        ()
        const
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
        
        ///////////////////////////////////////////////////////////////////////
        virtual ITestCase* clone
        ()
        const
        {
            return new OcMethod( m_cls, m_sel );
        }
        
        ///////////////////////////////////////////////////////////////////////
        virtual bool operator == 
        (
            const ITestCase& other
        )
        const
        {
            const OcMethod* ocmOther = dynamic_cast<const OcMethod*> ( &other );
            return ocmOther && ocmOther->m_sel == m_sel;
        }
        
        ///////////////////////////////////////////////////////////////////////
        virtual bool operator < 
        (
            const ITestCase& other
        )
        const
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
    
        ///////////////////////////////////////////////////////////////////////
        inline bool startsWith
        (
            const std::string& str, 
            const std::string& sub
        )
        {
            return str.length() > sub.length() && str.substr( 0, sub.length() ) == sub;
        }
        
        ///////////////////////////////////////////////////////////////////////
        inline const char* getAnnotation
        (
            Class cls, 
            const std::string& annotationName, 
            const std::string& testCaseName
        )
        {
            NSString* selStr = [[NSString alloc] initWithFormat:@"Catch_%s_%s", annotationName.c_str(), testCaseName.c_str()];
            SEL sel = NSSelectorFromString( selStr );
            [selStr release];
            if( [cls respondsToSelector: sel] )
                return (const char*)[cls performSelector: sel];
            return "";
        }        
    }
    
    ///////////////////////////////////////////////////////////////////////////
    inline size_t registerTestMethods
    ()
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
                        const char* name = Detail::getAnnotation( cls, "Name", testCaseName );
                        const char* desc = Detail::getAnnotation( cls, "Description", testCaseName );
                        
                        Hub::getTestCaseRegistry().registerTest( TestCaseInfo( new OcMethod( cls, selector ), name, desc, "", 0 ) );
                        noTestMethods++;
                        
                    }
                }
                free(methods);              
            }
        }
        return noTestMethods;
    }
    
    template<>
    inline std::string toString<NSString*>( NSString* const& nsstring )
    {
        return std::string( "@\"" ) + [nsstring UTF8String] + "\"";
    }
    
    namespace Matchers
    {
        namespace Impl
        {
        namespace NSStringMatchers
        {
            struct StringHolder
            {
                StringHolder( NSString* substr ) : m_substr( [substr copy] ){}
                StringHolder()
                {
                    [m_substr release];
                }
                
                NSString* m_substr;            
            };
            
            struct Contains : StringHolder
            {
                Contains( NSString* substr ) : StringHolder( substr ){}
                
                bool operator()( NSString* str ) const
                {
                    return [str rangeOfString:m_substr].location != NSNotFound;
                }
                
                friend std::ostream& operator<<( std::ostream& os, const Contains& matcher )
                {
                    os << "contains: " << Catch::toString( matcher.m_substr );
                    return os;
                }
            };

            struct StartsWith : StringHolder
            {
                StartsWith( NSString* substr ) : StringHolder( substr ){}
                
                bool operator()( NSString* str ) const
                {
                    return [str rangeOfString:m_substr].location == 0;
                }
                
                friend std::ostream& operator<<( std::ostream& os, const StartsWith& matcher )
                {
                    os << "starts with: " << Catch::toString( matcher.m_substr );
                    return os;
                }
            };
            struct EndsWith : StringHolder
            {
                EndsWith( NSString* substr ) : StringHolder( substr ){}
                
                bool operator()( NSString* str ) const
                {
                    return [str rangeOfString:m_substr].location == [str length] - [m_substr length];
                }
                
                friend std::ostream& operator<<( std::ostream& os, const EndsWith& matcher )
                {
                    os << "ends with: " << Catch::toString( matcher.m_substr );
                    return os;
                }
            };
            
        } // namespace NSStringMatchers
        } // namespace Impl
        
        inline Impl::NSStringMatchers::Contains
            Contains( NSString* substr ){ return Impl::NSStringMatchers::Contains( substr ); }
        inline Impl::NSStringMatchers::StartsWith
            StartsWith( NSString* substr ){ return Impl::NSStringMatchers::StartsWith( substr ); }
        inline Impl::NSStringMatchers::EndsWith
            EndsWith( NSString* substr ){ return Impl::NSStringMatchers::EndsWith( substr ); }
        
    } // namespace Matchers
    
    using namespace Matchers;
    
} // namespace Catch

///////////////////////////////////////////////////////////////////////////////
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