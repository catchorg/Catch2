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

#include <tuple>

#include <type_traits>

#include <string>

// We need a dummy global operator<< so we can bring it into Catch namespace later
struct Catch_global_namespace_dummy;
std::ostream& operator<<(std::ostream&, Catch_global_namespace_dummy);

namespace Catch {
    // Bring in operator<< from global namespace into Catch namespace
    using ::operator<<;

    namespace Detail {
        std::string rawMemoryToString( const void *object, std::size_t size );

        template<typename T>
        std::string rawMemoryToString( const T& object ) {
          return rawMemoryToString( &object, sizeof(object) );
        }

        template<typename T>
        class IsStreamInsertable {
            template<typename SS, typename TT>
            static auto test(int)
                -> decltype(std::declval<SS&>() << std::declval<TT>(), std::true_type());

            template<typename, typename>
            static auto test(...)->std::false_type;

        public:
            static const bool value = decltype(test<std::ostream, const T&>(0))::value;
        };
    
    } // namespace Detail

    // If we decide for C++14, change these to enable_if_ts
    template <typename T>
    struct StringMaker {
        template <typename Fake = T>
        typename std::enable_if<::Catch::Detail::IsStreamInsertable<Fake>::value, std::string>::type
            operator()(const Fake& t) {
            std::stringstream sstr;
            sstr << t;
            return sstr.str();
        }

        template <typename Fake = T>
        typename std::enable_if<!::Catch::Detail::IsStreamInsertable<Fake>::value, std::string>::type
            operator()(const Fake&) {
            return "{?}";
        }
    };

    namespace Detail {

        extern const std::string unprintableString;

        // This function dispatches all stringification requests inside of Catch.
        // Should be preferably called fully qualified, like ::Catch::Detail::stringify
        template <typename T>
        std::string stringify(const T& e) {
            return ::Catch::StringMaker<typename std::remove_cv<typename std::remove_reference<T>::type>::type>{}(e);
        }

    } // namespace Detail


    // Some predefined specializations

    template<>
    struct StringMaker<std::string> {
        std::string operator()(const std::string& str);
    };
    template<>
    struct StringMaker<std::wstring> {
        std::string operator()(const std::wstring& wstr);
    };

    template<>
    struct StringMaker<char const *> {
        std::string operator()(char const * str);
    };
    template<>
    struct StringMaker<char *> {
        std::string operator()(char * str);
    };
    template<>
    struct StringMaker<wchar_t const *> {
        std::string operator()(wchar_t const * str);
    };
    template<>
    struct StringMaker<wchar_t *> {
        std::string operator()(wchar_t * str);
    };

    template<int SZ>
    struct StringMaker<char[SZ]> {
        std::string operator()(const char* str) {
            return ::Catch::Detail::stringify(std::string{ str });
        }
    };
    template<int SZ>
    struct StringMaker<signed char[SZ]> {
        std::string operator()(const char* str) {
            return ::Catch::Detail::stringify(std::string{ str });
        }
    };
    template<int SZ>
    struct StringMaker<unsigned char[SZ]> {
        std::string operator()(const char* str) {
            return ::Catch::Detail::stringify(std::string{ str });
        }
    };

    template<>
    struct StringMaker<int> {
        std::string operator()(int value);
    };
    template<>
    struct StringMaker<long> {
        std::string operator()(long value);
    };
    template<>
    struct StringMaker<long long> {
        std::string operator()(long long value);
    };
    template<>
    struct StringMaker<unsigned int> {
        std::string operator()(unsigned int value);
    };
    template<>
    struct StringMaker<unsigned long> {
        std::string operator()(unsigned long value);
    };
    template<>
    struct StringMaker<unsigned long long> {
        std::string operator()(unsigned long long value);
    };

    template<>
    struct StringMaker<bool> {
        std::string operator()(bool b);
    };

    template<>
    struct StringMaker<char> {
        std::string operator()(char c);
    };
    template<>
    struct StringMaker<signed char> {
        std::string operator()(signed char c);
    };
    template<>
    struct StringMaker<unsigned char> {
        std::string operator()(unsigned char c);
    };

    template<>
    struct StringMaker<nullptr_t> {
        std::string operator()(std::nullptr_t);
    };

    template<>
    struct StringMaker<float> {
        std::string operator()(float value);
    };
    template<>
    struct StringMaker<double> {
        std::string operator()(double value);
    };

    template <typename T>
    struct StringMaker<T*> {
        template <typename U>
        std::string operator()(U* p) {
            if (p) {
                return ::Catch::Detail::rawMemoryToString(p);
            } else {
                return "nullptr";
            }
        }
    };

    template <typename R, typename C>
    struct StringMaker<R C::*> {
        std::string operator()(R C::* p) {
            if (p) {
                return ::Catch::Detail::rawMemoryToString(p);
            } else {
                return "nullptr";
            }
        }
    };

    namespace Detail {
        template<typename InputIterator>
        std::string rangeToString(InputIterator first, InputIterator last) {
            std::ostringstream oss;
            oss << "{ ";
            if (first != last) {
                oss << ::Catch::Detail::stringify(*first);
                for (++first; first != last; ++first)
                    oss << ", " << ::Catch::Detail::stringify(*first);
            }
            oss << " }";
            return oss.str();
        }
    }

    template<typename T, typename Allocator>
    struct StringMaker<std::vector<T, Allocator> > {
        std::string operator()( std::vector<T,Allocator> const& v ) {
            return ::Catch::Detail::rangeToString( v.begin(), v.end() );
        }
    };

    // === Pair ===
    template<typename T1, typename T2>
    struct StringMaker<std::pair<T1, T2> > {
        std::string operator()(const std::pair<T1, T2>& pair) {
            std::ostringstream oss;
            oss << "{ "
                << ::Catch::Detail::stringify(pair.first)
                << ", "
                << ::Catch::Detail::stringify(pair.second)
                << " }";
            return oss.str();
        }
    };



    namespace Detail {
        template<
            typename Tuple,
            std::size_t N = 0,
            bool = (N < std::tuple_size<Tuple>::value)
            >
            struct TupleElementPrinter {
            static void print(const Tuple& tuple, std::ostream& os) {
                os << (N ? ", " : " ")
                    << ::Catch::Detail::stringify(std::get<N>(tuple));
                TupleElementPrinter<Tuple, N + 1>::print(tuple, os);
            }
        };

        template<
            typename Tuple,
            std::size_t N
        >
            struct TupleElementPrinter<Tuple, N, false> {
            static void print(const Tuple&, std::ostream&) {}
        };

    }


    template<typename ...Types>
    struct StringMaker<std::tuple<Types...>> {
        std::string operator()(const std::tuple<Types...>& tuple) {
            std::ostringstream os;
            os << '{';
            Detail::TupleElementPrinter<std::tuple<Types...>>::print(tuple, os);
            os << " }";
            return os.str();
        }
    };


    template<typename T>
    struct EnumStringMaker {
        std::string operator()(const T& t) {
            return ::Catch::Detail::stringify(static_cast<typename std::underlying_type<T>::type>(t));
        }
    };

#ifdef __OBJC__
    template<>
    struct StringMaker<NSString const *> {
        std::string operator()(NSString const* const& nsstring);
    };
    template<>
    struct StringMaker<NSString * CATCH_ARC_STRONG> {
        std::string operator()(NSString * CATCH_ARC_STRONG const& nsstring);
    };
    template<>
    struct StringMaker<NSObject *> {
        std::string operator()(NSObject* const& nsObject);
    };
#endif


} // namespace Catch

#endif // TWOBLUECUBES_CATCH_TOSTRING_H_INCLUDED
