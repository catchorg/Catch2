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

#ifdef __has_feature
#define CATCH_ARC_ENABLED __has_feature(objc_arc)
#else
#define CATCH_ARC_ENABLED 0
#endif

void arcSafeRelease( NSObject* obj );
id performOptionalSelector( id obj, SEL sel );

#if !CATCH_ARC_ENABLED
    inline void arcSafeRelease( NSObject* obj )
    {
        [obj release];
    }
    inline id performOptionalSelector( id obj, SEL sel )
    {
        if( [obj respondsToSelector: sel] )
            return [obj performSelector: sel];
        return nil;
    }
    #define CATCH_UNSAFE_UNRETAINED
#else
    inline void arcSafeRelease( NSObject* ){}
    inline id performOptionalSelector( id obj, SEL sel )
    {
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Warc-performSelector-leaks"
        if( [obj respondsToSelector: sel] )
            return [obj performSelector: sel];
    #pragma clang diagnostic pop     
        return nil;
    }
    #define CATCH_UNSAFE_UNRETAINED __unsafe_unretained
#endif

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
            id obj = [[m_cls alloc] init];
            
            performOptionalSelector( obj, @selector(setUp)  );
            performOptionalSelector( obj, m_sel );
            performOptionalSelector( obj, @selector(tearDown)  );
            
            arcSafeRelease( obj );
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
        inline std::string getAnnotation
        (
            Class cls, 
            const std::string& annotationName, 
            const std::string& testCaseName
        )
        {
            NSString* selStr = [[NSString alloc] initWithFormat:@"Catch_%s_%s", annotationName.c_str(), testCaseName.c_str()];
            SEL sel = NSSelectorFromString( selStr );
            arcSafeRelease( selStr );
            id value = performOptionalSelector( cls, sel );
            if( value )
                return [(NSString*)value UTF8String];
            return "";
        }        
    }
    
    ///////////////////////////////////////////////////////////////////////////
    inline size_t registerTestMethods
    ()
    {
        size_t noTestMethods = 0;
        int noClasses = objc_getClassList( NULL, 0 );
        
        Class* classes = (CATCH_UNSAFE_UNRETAINED Class *)malloc( sizeof(Class) * noClasses);
        objc_getClassList( classes, noClasses );
        
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
                        
                        Context::getTestCaseRegistry().registerTest( TestCaseInfo( new OcMethod( cls, selector ), name.c_str(), desc.c_str(), SourceLineInfo() ) );
                        noTestMethods++;
                        
                    }
                }
                free(methods);              
            }
        }
        return noTestMethods;
    }
    
    inline std::string toString( NSString* const& nsstring )
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
                    arcSafeRelease( m_substr );
                }
                
                NSString* m_substr;            
            };
            
            struct Equals : StringHolder
            {
                Equals( NSString* substr ) : StringHolder( substr ){}
                
                bool operator()( NSString* str ) const
                {
                    return [str isEqualToString:m_substr];
                }
                
                friend std::ostream& operator<<( std::ostream& os, const Equals& matcher )
                {
                    os << "equals string: " << Catch::toString( matcher.m_substr );
                    return os;
                }
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
        
        inline Impl::NSStringMatchers::Equals
            Equals( NSString* substr ){ return Impl::NSStringMatchers::Equals( substr ); }
        
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
+(NSString*) INTERNAL_CATCH_UNIQUE_NAME( Catch_Name_test ) \
{\
return @ name; \
}\
+(NSString*) INTERNAL_CATCH_UNIQUE_NAME( Catch_Description_test ) \
{ \
return @ desc; \
} \
-(void) INTERNAL_CATCH_UNIQUE_NAME( Catch_TestCase_test )

#endif // TWOBLUECUBES_CATCH_OBJC_HPP_INCLUDED