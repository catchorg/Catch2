/*
 *  Created by Phil on 9/8/2017
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CAPTURE_MATCHERS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CAPTURE_MATCHERS_HPP_INCLUDED

#include "catch_capture.hpp"
#include "catch_matchers.h"
#include "catch_matchers_string.h"
#include "catch_matchers_vector.h"

namespace Catch {

    template<typename ArgT, typename MatcherT>
    class MatchExpr : public ITransientExpression {
        ArgT const& m_arg;
        MatcherT m_matcher;
        StringRef m_matcherString;
        bool m_result;
    public:
        MatchExpr( ArgT const& arg, MatcherT const& matcher, StringRef matcherString )
        :   m_arg( arg ),
            m_matcher( matcher ),
            m_matcherString( matcherString ),
            m_result( matcher.match( arg ) )
        {}

        auto isBinaryExpression() const -> bool  override { return true; }
        auto getResult() const -> bool override { return m_result; }

        void streamReconstructedExpression( std::ostream &os ) const override {
            auto matcherAsString = m_matcher.toString();
            os << Catch::Detail::stringify( m_arg ) << ' ';
            if( matcherAsString == Detail::unprintableString )
                os << m_matcherString;
            else
                os << matcherAsString;
        }
    };

    using StringMatcher = Matchers::Impl::MatcherBase<std::string>;

    void handleExceptionMatchExpr( AssertionHandler& handler, StringMatcher const& matcher, StringRef matcherString  );

    template<typename ArgT, typename MatcherT>
    auto makeMatchExpr( ArgT const& arg, MatcherT const& matcher, StringRef matcherString  ) -> MatchExpr<ArgT, MatcherT> {
        return MatchExpr<ArgT, MatcherT>( arg, matcher, matcherString );
    }

} // namespace Catch


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( macroName, matcher, resultDisposition, arg ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(arg) ", " CATCH_INTERNAL_STRINGIFY(matcher), resultDisposition ); \
        INTERNAL_CATCH_TRY( catchAssertionHandler ) { \
            catchAssertionHandler.handle( Catch::makeMatchExpr( arg, matcher, #matcher ) ); \
        } INTERNAL_CATCH_CATCH( catchAssertionHandler ) \
        INTERNAL_CATCH_REACT( catchAssertionHandler ) \
    } while( Catch::alwaysFalse() )


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_MATCHES( macroName, exceptionType, resultDisposition, matcher, ... ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(__VA_ARGS__) ", " CATCH_INTERNAL_STRINGIFY(exceptionType) ", " CATCH_INTERNAL_STRINGIFY(matcher), resultDisposition ); \
        if( catchAssertionHandler.allowThrows() ) \
            try { \
                static_cast<void>(__VA_ARGS__ ); \
                catchAssertionHandler.handle( Catch::ResultWas::DidntThrowException ); \
            } \
            catch( exceptionType const& ex ) { \
                catchAssertionHandler.handle( Catch::makeMatchExpr( ex, matcher, #matcher ) ); \
            } \
            catch( ... ) { \
                catchAssertionHandler.useActiveException(); \
            } \
        else \
            catchAssertionHandler.handle( Catch::ResultWas::Ok ); \
        INTERNAL_CATCH_REACT( catchAssertionHandler ) \
    } while( Catch::alwaysFalse() )

#endif // TWOBLUECUBES_CATCH_CAPTURE_MATCHERS_HPP_INCLUDED
