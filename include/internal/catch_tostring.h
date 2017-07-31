/*
 *  Created by Phil on 8/5/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TOSTRING_H_INCLUDED
#define TWOBLUECUBES_CATCH_TOSTRING_H_INCLUDED

#include "catch_common.h"

#include <sstream>
#include <iomanip>
#include <limits>
#include <vector>
#include <cstddef>

#ifdef __OBJC__
#include "catch_objc_arc.hpp"
#endif

#ifdef CATCH_CONFIG_CPP11_TUPLE
#include <tuple>
#endif

#ifdef CATCH_CONFIG_CPP11_IS_ENUM
#include <type_traits>
#endif

namespace Catch {

// Why we're here.
template<typename T>
std::string toString( T const& value );

// Built in overloads

std::string toString( std::string const& value );
std::string toString( std::wstring const& value );
std::string toString( const char* const value );
std::string toString( char* const value );
std::string toString( const wchar_t* const value );
std::string toString( wchar_t* const value );
std::string toString( int value );
std::string toString( unsigned long value );
std::string toString( unsigned int value );
std::string toString( const double value );
std::string toString( const float value );
std::string toString( bool value );
std::string toString( char value );
std::string toString( signed char value );
std::string toString( unsigned char value );

#ifdef CATCH_CONFIG_CPP11_LONG_LONG
std::string toString( long long value );
std::string toString( unsigned long long value );
#endif

#ifdef CATCH_CONFIG_CPP11_NULLPTR
std::string toString( std::nullptr_t );
#endif

#ifdef __OBJC__
    std::string toString( NSString const * const& nsstring );
    std::string toString( NSString * CATCH_ARC_STRONG & nsstring );
    std::string toString( NSObject* const& nsObject );
#endif


namespace Detail {

    extern const std::string unprintableString;

 #if !defined(CATCH_CONFIG_CPP11_STREAM_INSERTABLE_CHECK)
    struct BorgType {
        template<typename T> BorgType( T const& );
    };

    struct TrueType { char sizer[1]; };
    struct FalseType { char sizer[2]; };

    TrueType& testStreamable( std::ostream& );
    FalseType testStreamable( FalseType );

    FalseType operator<<( std::ostream const&, BorgType const& );

    template<typename T>
    struct IsStreamInsertable {
        static std::ostream &s;
        static T  const&t;
        enum { value = sizeof( testStreamable(s << t) ) == sizeof( TrueType ) };
    };
#else
    template<typename T>
    class IsStreamInsertable {
        template<typename SS, typename TT>
        static auto test(int)
        -> decltype( std::declval<SS&>() << std::declval<TT>(), std::true_type() );

        template<typename, typename>
        static auto test(...) -> std::false_type;

    public:
        static const bool value = decltype(test<std::ostream,const T&>(0))::value;
    };
#endif

#if defined(CATCH_CONFIG_CPP11_IS_ENUM)
    template<typename T,
             bool IsEnum = std::is_enum<T>::value
             >
    struct EnumStringMaker
    {
        static std::string convert( T const& ) { return unprintableString; }
    };

    template<typename T>
    struct EnumStringMaker<T,true>
    {
        static std::string convert( T const& v )
        {
            return ::Catch::toString(
                static_cast<typename std::underlying_type<T>::type>(v)
                );
        }
    };
#endif
    template<bool C>
    struct StringMakerBase {
#if defined(CATCH_CONFIG_CPP11_IS_ENUM)
        template<typename T>
        static std::string convert( T const& v )
        {
            return EnumStringMaker<T>::convert( v );
        }
#else
        template<typename T>
        static std::string convert( T const& ) { return unprintableString; }
#endif
    };

    template<>
    struct StringMakerBase<true> {
        template<typename T>
        static std::string convert( T const& _value ) {
            std::ostringstream oss;
            oss << _value;
            return oss.str();
        }
    };

    std::string rawMemoryToString( const void *object, std::size_t size );

    template<typename T>
    std::string rawMemoryToString( const T& object ) {
      return rawMemoryToString( &object, sizeof(object) );
    }

} // end namespace Detail

template<typename T>
struct StringMaker :
    Detail::StringMakerBase<Detail::IsStreamInsertable<T>::value> {};

template<typename T>
struct StringMaker<T*> {
    template<typename U>
    static std::string convert( U* p ) {
        if( !p )
            return "NULL";
        else
            return Detail::rawMemoryToString( p );
    }
};

template<typename R, typename C>
struct StringMaker<R C::*> {
    static std::string convert( R C::* p ) {
        if( !p )
            return "NULL";
        else
            return Detail::rawMemoryToString( p );
    }
};

namespace Detail {
    template<typename InputIterator>
    std::string rangeToString( InputIterator first, InputIterator last );
}

//template<typename T, typename Allocator>
//struct StringMaker<std::vector<T, Allocator> > {
//    static std::string convert( std::vector<T,Allocator> const& v ) {
//        return Detail::rangeToString( v.begin(), v.end() );
//    }
//};

template<typename T, typename Allocator>
std::string toString( std::vector<T,Allocator> const& v ) {
    return Detail::rangeToString( v.begin(), v.end() );
}


#ifdef CATCH_CONFIG_CPP11_TUPLE

// toString for tuples
namespace TupleDetail {
  template<
      typename Tuple,
      std::size_t N = 0,
      bool = (N < std::tuple_size<Tuple>::value)
      >
  struct ElementPrinter {
      static void print( const Tuple& tuple, std::ostream& os )
      {
          os << ( N ? ", " : " " )
             << Catch::toString(std::get<N>(tuple));
          ElementPrinter<Tuple,N+1>::print(tuple,os);
      }
  };

  template<
      typename Tuple,
      std::size_t N
      >
  struct ElementPrinter<Tuple,N,false> {
      static void print( const Tuple&, std::ostream& ) {}
  };

}

template<typename ...Types>
struct StringMaker<std::tuple<Types...>> {

    static std::string convert( const std::tuple<Types...>& tuple )
    {
        std::ostringstream os;
        os << '{';
        TupleDetail::ElementPrinter<std::tuple<Types...>>::print( tuple, os );
        os << " }";
        return os.str();
    }
};
#endif // CATCH_CONFIG_CPP11_TUPLE

namespace Detail {
    template<typename T>
    std::string makeString( T const& value ) {
        return StringMaker<T>::convert( value );
    }
} // end namespace Detail

/// \brief converts any type to a string
///
/// The default template forwards on to ostringstream - except when an
/// ostringstream overload does not exist - in which case it attempts to detect
/// that and writes {?}.
/// Overload (not specialise) this template for custom typs that you don't want
/// to provide an ostream overload for.
template<typename T>
std::string toString( T const& value ) {
    return StringMaker<T>::convert( value );
}


    namespace Detail {
    template<typename InputIterator>
    std::string rangeToString( InputIterator first, InputIterator last ) {
        std::ostringstream oss;
        oss << "{ ";
        if( first != last ) {
            oss << Catch::toString( *first );
            for( ++first ; first != last ; ++first )
                oss << ", " << Catch::toString( *first );
        }
        oss << " }";
        return oss.str();
    }
}

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TOSTRING_H_INCLUDED
