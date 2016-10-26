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
#include <cstddef>
#include <iterator>

#ifndef CATCH_CPP11_OR_GREATER
#include <vector>
#endif

#ifdef __OBJC__
#include "catch_objc_arc.hpp"
#endif

#ifdef CATCH_CONFIG_CPP11_TUPLE
#include <tuple>
#endif

#ifdef CATCH_CPP11_OR_GREATER
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
    std::string toString( NSString * CATCH_ARC_STRONG const& nsstring );
    std::string toString( NSObject* const& nsObject );
#endif


namespace Detail {

    extern const std::string unprintableString;

    struct BorgType {
        template<typename T> BorgType( T const& );
    };

    // Without c++11's `decltype`, rely on function overloading returning
    // different sized types that can be discriminated with `sizeof`.
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
    inline std::string rawMemoryToString( const T& object ) {
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

#ifndef CATCH_CPP11_OR_GREATER
template<typename T, typename Allocator>
std::string toString( std::vector<T,Allocator> const& v ) {
    return Detail::rangeToString( v.begin(), v.end() );
}

#else
 
namespace Detail {
// Importing c++11 std instead of prefixing call by std:: to allow for user
// custom overload find through ADL
using std::begin;
using std::end;

template<typename T>
struct HasBasicContainerApi
{
    /// This overload will be chosen if all the operations in it's signature
    /// can be resolved at compile time.
    template<typename C,
             // Test const_iterator lvalue supports operator++
             class = decltype(++std::declval<typename C::const_iterator&>()),
             // Test that begin and end return comparable values
             class = decltype(begin(std::declval<const C>())
                              != end(std::declval<const C>()))>
    static std::true_type has( typename C::const_iterator*, typename C::value_type*);
    /// Lower priority overload will be picked if the previous one fails.
    template<typename C>
    static std::false_type has( ... );
    static const bool value = decltype(has<T>(NULL, NULL))::value;
};

// Compile type function that returns true if a type matches
// the container concept (http://en.cppreference.com/w/cpp/concept/Container),
// false otherwise.
template <class C, bool = HasBasicContainerApi<C>::value>
struct IsContainer {
    // C array are containers although not having the standard api.
    static const bool value = std::is_array<C>::value;
};

// Specialization for types that have a basic container API
// that can now be checked for type inconsistency.
template <class C>
struct IsContainer<C, true> {
private:
    typedef typename C::value_type value_type;
    typedef typename C::const_iterator const_iterator;
    static const C& c;
public:
    static const bool value =
        std::is_same<const_iterator, decltype( begin(c) )>::value &&
        std::is_same<const_iterator, decltype( end(c) )>::value &&
        std::is_same<const value_type&, decltype( *begin(c) )>::value &&
        std::is_same<const value_type&, decltype( *end(c) )>::value;
};

/// Print containers by printing their elements in succession
template<typename C,
         class = typename std::enable_if<Detail::IsContainer<C>::value>::type,
         class = void>
std::string toStringInternal( C const& c ) {
    return Detail::rangeToString( begin(c), end(c) );
}

} // end namespace Detail

#endif // CATCH_CPP11_OR_GREATER

// toString for pairs
template<typename T1, typename T2>
struct StringMaker<std::pair<T1,T2> > {
    static std::string convert( const std::pair<T1,T2>& pair ) {
        std::ostringstream oss;
        oss << "{ "
            << toString( pair.first )
            << ", "
            << toString( pair.second )
            << " }";
        return oss.str();
    }
};

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

/// Instead of adding complex template overloading of public toString method,
/// use an internal dispatcher which template can get as complicated as needed
/// without impacting the public api.
template<typename T
#if defined(CATCH_CPP11_OR_GREATER)
         , class = typename std::enable_if<!Detail::IsContainer<T>::value>::type
#endif
        >
std::string toStringInternal( T const& value ) {
    return StringMaker<T>::convert( value );
}

} // end namespace Detail

/// \brief converts any type to a string
///
/// The default template forwards on to ostringstream - except when an
/// ostringstream overload does not exist - in which case it attempts to detect
/// that and writes {?}.
/// Overload (not specialize) this template for custom types that you don't want
/// to provide an ostream overload for.
template<typename T>
std::string toString( T const& value ) {
    return Detail::toStringInternal(value);
}


namespace Detail {
template<typename InputIterator>
std::string rangeToString( InputIterator first, InputIterator last ) {
    std::ostringstream oss;
    oss << "{ ";
    if( first != last ) {
        oss << Catch::toString( *first );
        while( ++first != last )
            oss << ", " << Catch::toString( *first );
    }
    oss << " }";
    return oss.str();
}
}

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TOSTRING_H_INCLUDED
