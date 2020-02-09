/*
 *  Created by Phil on 9/8/2017
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CAPTURE_MATCHERS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CAPTURE_MATCHERS_HPP_INCLUDED

#include <catch2/catch_capture.hpp>
#include <catch2/catch_matchers.h>
#include <catch2/catch_stringref.h>
#include <catch2/catch_interfaces_registry_hub.h>

namespace Catch {

    template<typename ArgT, typename MatcherT>
    class MatchExpr : public ITransientExpression {
        ArgT m_arg;
        MatcherT m_matcher;
        StringRef m_matcherString;
    public:
        MatchExpr( ArgT arg, MatcherT const& matcher, StringRef const& matcherString )
        :   ITransientExpression{ true, matcher.match( arg ) },
            m_arg( std::forward<ArgT>(arg) ),
            m_matcher( matcher ),
            m_matcherString( matcherString )
        {}

        void streamReconstructedExpression( std::ostream &os ) const override {
            auto matcherAsString = m_matcher.toString();
            os << Catch::Detail::stringify( m_arg ) << ' ';
            if( matcherAsString == Detail::unprintableString )
                os << m_matcherString;
            else
                os << matcherAsString;
        }
    };

    // This is the general overload that takes a any string matcher
    // There is another overload, in catch_assertionhandler.h/.cpp, that only takes a string and infers
    // the Equals matcher (so the header does not mention matchers)
    template<typename StringMatcher,
        typename = typename std::enable_if<std::is_base_of<MatcherBaseGeneric<StringMatcher>, StringMatcher>::value>::type>
    void handleExceptionMatchExpr( AssertionHandler& handler, StringMatcher const& matcher, StringRef const& matcherString  ) {
        std::string exceptionMessage = Catch::translateActiveException();
        MatchExpr<std::string const&, StringMatcher const&> expr( exceptionMessage, matcher, matcherString );
        handler.handleExpr( expr );
    }

    template<typename ArgT, typename MatcherT>
    auto makeMatchExpr( ArgT && arg, MatcherT const& matcher, StringRef const& matcherString  ) -> MatchExpr<ArgT&&, MatcherT> {
        return MatchExpr<ArgT&&, MatcherT>( std::forward<ArgT>(arg), matcher, matcherString );
    }

} // namespace Catch


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( macroName, matcher, resultDisposition, arg ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName##_catch_sr, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(arg) ", " CATCH_INTERNAL_STRINGIFY(matcher), resultDisposition ); \
        INTERNAL_CATCH_TRY { \
            catchAssertionHandler.handleExpr( Catch::makeMatchExpr( arg, matcher, #matcher##_catch_sr ) ); \
        } INTERNAL_CATCH_CATCH( catchAssertionHandler ) \
        INTERNAL_CATCH_REACT( catchAssertionHandler ) \
    } while( false )


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_MATCHES( macroName, exceptionType, resultDisposition, matcher, ... ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName##_catch_sr, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(__VA_ARGS__) ", " CATCH_INTERNAL_STRINGIFY(exceptionType) ", " CATCH_INTERNAL_STRINGIFY(matcher), resultDisposition ); \
        if( catchAssertionHandler.allowThrows() ) \
            try { \
                static_cast<void>(__VA_ARGS__ ); \
                catchAssertionHandler.handleUnexpectedExceptionNotThrown(); \
            } \
            catch( exceptionType const& ex ) { \
                catchAssertionHandler.handleExpr( Catch::makeMatchExpr( ex, matcher, #matcher##_catch_sr ) ); \
            } \
            catch( ... ) { \
                catchAssertionHandler.handleUnexpectedInflightException(); \
            } \
        else \
            catchAssertionHandler.handleThrowingCallSkipped(); \
        INTERNAL_CATCH_REACT( catchAssertionHandler ) \
    } while( false )

#endif // TWOBLUECUBES_CATCH_CAPTURE_MATCHERS_HPP_INCLUDED
