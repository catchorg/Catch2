/*
 *  Created by Phil Nash on 04/03/2012.
 *  Copyright (c) 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED

namespace Catch {
namespace Matchers {
    namespace Impl {

    namespace Generic {
        template<typename ExpressionT> class AllOf;
        template<typename ExpressionT> class AnyOf;
        template<typename ExpressionT> class Not;
    }

    template<typename ExpressionT>
    struct Matcher : SharedImpl<IShared>
    {
        typedef ExpressionT ExpressionType;

        virtual ~Matcher() {}
        virtual Ptr<Matcher> clone() const = 0;
        virtual bool match( ExpressionT const& expr ) const = 0;
        virtual std::string toString() const = 0;

        Generic::AllOf<ExpressionT> operator && ( Matcher<ExpressionT> const& other ) const;
        Generic::AnyOf<ExpressionT> operator || ( Matcher<ExpressionT> const& other ) const;
        Generic::Not<ExpressionT> operator ! () const;
    };

    template<typename DerivedT, typename ExpressionT>
    struct MatcherImpl : Matcher<ExpressionT> {

        virtual Ptr<Matcher<ExpressionT> > clone() const {
            return Ptr<Matcher<ExpressionT> >( new DerivedT( static_cast<DerivedT const&>( *this ) ) );
        }
    };

    namespace Generic {
        template<typename ExpressionT>
        class Not : public MatcherImpl<Not<ExpressionT>, ExpressionT> {
        public:
            explicit Not( Matcher<ExpressionT> const& matcher ) : m_matcher(matcher.clone()) {}
            Not( Not const& other ) : m_matcher( other.m_matcher ) {}

            virtual bool match( ExpressionT const& expr ) const CATCH_OVERRIDE {
                return !m_matcher->match( expr );
            }

            virtual std::string toString() const CATCH_OVERRIDE {
                return "not " + m_matcher->toString();
            }
        private:
            Ptr< Matcher<ExpressionT> > m_matcher;
        };

        template<typename ExpressionT>
        class AllOf : public MatcherImpl<AllOf<ExpressionT>, ExpressionT> {
        public:

            AllOf() {}
            AllOf( AllOf const& other ) : m_matchers( other.m_matchers ) {}

            AllOf& add( Matcher<ExpressionT> const& matcher ) {
                m_matchers.push_back( matcher.clone() );
                return *this;
            }
            virtual bool match( ExpressionT const& expr ) const
            {
                for( std::size_t i = 0; i < m_matchers.size(); ++i )
                    if( !m_matchers[i]->match( expr ) )
                        return false;
                return true;
            }
            virtual std::string toString() const {
                std::ostringstream oss;
                oss << "( ";
                for( std::size_t i = 0; i < m_matchers.size(); ++i ) {
                    if( i != 0 )
                        oss << " and ";
                    oss << m_matchers[i]->toString();
                }
                oss << " )";
                return oss.str();
            }

            AllOf operator && ( Matcher<ExpressionT> const& other ) const {
                AllOf allOfExpr( *this );
                allOfExpr.add( other );
                return allOfExpr;
            }

        private:
            std::vector<Ptr<Matcher<ExpressionT> > > m_matchers;
        };

        template<typename ExpressionT>
        class AnyOf : public MatcherImpl<AnyOf<ExpressionT>, ExpressionT> {
        public:

            AnyOf() {}
            AnyOf( AnyOf const& other ) : m_matchers( other.m_matchers ) {}

            AnyOf& add( Matcher<ExpressionT> const& matcher ) {
                m_matchers.push_back( matcher.clone() );
                return *this;
            }
            virtual bool match( ExpressionT const& expr ) const
            {
                for( std::size_t i = 0; i < m_matchers.size(); ++i )
                    if( m_matchers[i]->match( expr ) )
                        return true;
                return false;
            }
            virtual std::string toString() const {
                std::ostringstream oss;
                oss << "( ";
                for( std::size_t i = 0; i < m_matchers.size(); ++i ) {
                    if( i != 0 )
                        oss << " or ";
                    oss << m_matchers[i]->toString();
                }
                oss << " )";
                return oss.str();
            }

            AnyOf operator || ( Matcher<ExpressionT> const& other ) const {
                AnyOf anyOfExpr( *this );
                anyOfExpr.add( other );
                return anyOfExpr;
            }

        private:
            std::vector<Ptr<Matcher<ExpressionT> > > m_matchers;
        };

    } // namespace Generic

    template<typename ExpressionT>
    Generic::AllOf<ExpressionT> Matcher<ExpressionT>::operator && ( Matcher<ExpressionT> const& other ) const {
        Generic::AllOf<ExpressionT> allOfExpr;
        allOfExpr.add( *this );
        allOfExpr.add( other );
        return allOfExpr;
    }

    template<typename ExpressionT>
    Generic::AnyOf<ExpressionT> Matcher<ExpressionT>::operator || ( Matcher<ExpressionT> const& other ) const {
        Generic::AnyOf<ExpressionT> anyOfExpr;
        anyOfExpr.add( *this );
        anyOfExpr.add( other );
        return anyOfExpr;
    }

    template<typename ExpressionT>
    Generic::Not<ExpressionT> Matcher<ExpressionT>::operator ! () const {
        return Generic::Not<ExpressionT>( *this );
    }


    namespace StdString {

        inline std::string makeString( std::string const& str ) { return str; }
        inline std::string makeString( const char* str ) { return str ? std::string( str ) : std::string(); }

        struct CasedString
        {
            CasedString( std::string const& str, CaseSensitive::Choice caseSensitivity )
            :   m_caseSensitivity( caseSensitivity ),
                m_str( adjustString( str ) )
            {}
            std::string adjustString( std::string const& str ) const {
                return m_caseSensitivity == CaseSensitive::No
                    ? toLower( str )
                    : str;

            }
            std::string toStringSuffix() const
            {
                return m_caseSensitivity == CaseSensitive::No
                    ? " (case insensitive)"
                    : "";
            }
            CaseSensitive::Choice m_caseSensitivity;
            std::string m_str;
        };

        struct Equals : MatcherImpl<Equals, std::string> {
            Equals( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes )
            :   m_data( str, caseSensitivity )
            {}
            Equals( Equals const& other ) : m_data( other.m_data ){}

            virtual ~Equals();

            virtual bool match( std::string const& expr ) const {
                return m_data.m_str == m_data.adjustString( expr );;
            }
            virtual std::string toString() const {
                return "equals: \"" + m_data.m_str + "\"" + m_data.toStringSuffix();
            }

            CasedString m_data;
        };

        struct Contains : MatcherImpl<Contains, std::string> {
            Contains( std::string const& substr, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes )
            : m_data( substr, caseSensitivity ){}
            Contains( Contains const& other ) : m_data( other.m_data ){}

            virtual ~Contains();

            virtual bool match( std::string const& expr ) const {
                return m_data.adjustString( expr ).find( m_data.m_str ) != std::string::npos;
            }
            virtual std::string toString() const {
                return "contains: \"" + m_data.m_str  + "\"" + m_data.toStringSuffix();
            }

            CasedString m_data;
        };

        struct StartsWith : MatcherImpl<StartsWith, std::string> {
            StartsWith( std::string const& substr, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes )
            : m_data( substr, caseSensitivity ){}

            StartsWith( StartsWith const& other ) : m_data( other.m_data ){}

            virtual ~StartsWith();

            virtual bool match( std::string const& expr ) const {
                return startsWith( m_data.adjustString( expr ), m_data.m_str );
            }
            virtual std::string toString() const {
                return "starts with: \"" + m_data.m_str + "\"" + m_data.toStringSuffix();
            }

            CasedString m_data;
        };

        struct EndsWith : MatcherImpl<EndsWith, std::string> {
            EndsWith( std::string const& substr, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes )
            : m_data( substr, caseSensitivity ){}
            EndsWith( EndsWith const& other ) : m_data( other.m_data ){}

            virtual ~EndsWith();

            virtual bool match( std::string const& expr ) const {
                return endsWith( m_data.adjustString( expr ), m_data.m_str );
            }
            virtual std::string toString() const {
                return "ends with: \"" + m_data.m_str + "\"" + m_data.toStringSuffix();
            }

            CasedString m_data;
        };
    } // namespace StdString
    } // namespace Impl

    // The following functions create the actual matcher objects.
    // This allows the types to be inferred
    template<typename ExpressionT>
    inline Impl::Generic::Not<ExpressionT> Not( Impl::Matcher<ExpressionT> const& m ) {
        return Impl::Generic::Not<ExpressionT>( m );
    }

    template<typename ExpressionT>
    inline Impl::Generic::AllOf<ExpressionT> AllOf( Impl::Matcher<ExpressionT> const& m1,
                                                    Impl::Matcher<ExpressionT> const& m2 ) {
        return Impl::Generic::AllOf<ExpressionT>().add( m1 ).add( m2 );
    }
    template<typename ExpressionT>
    inline Impl::Generic::AllOf<ExpressionT> AllOf( Impl::Matcher<ExpressionT> const& m1,
                                                    Impl::Matcher<ExpressionT> const& m2,
                                                    Impl::Matcher<ExpressionT> const& m3 ) {
        return Impl::Generic::AllOf<ExpressionT>().add( m1 ).add( m2 ).add( m3 );
    }
    template<typename ExpressionT>
    inline Impl::Generic::AnyOf<ExpressionT> AnyOf( Impl::Matcher<ExpressionT> const& m1,
                                                    Impl::Matcher<ExpressionT> const& m2 ) {
        return Impl::Generic::AnyOf<ExpressionT>().add( m1 ).add( m2 );
    }
    template<typename ExpressionT>
    inline Impl::Generic::AnyOf<ExpressionT> AnyOf( Impl::Matcher<ExpressionT> const& m1,
                                                    Impl::Matcher<ExpressionT> const& m2,
                                                    Impl::Matcher<ExpressionT> const& m3 ) {
        return Impl::Generic::AnyOf<ExpressionT>().add( m1 ).add( m2 ).add( m3 );
    }

    inline Impl::StdString::Equals      Equals( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes ) {
        return Impl::StdString::Equals( str, caseSensitivity );
    }
    inline Impl::StdString::Equals      Equals( const char* str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes ) {
        return Impl::StdString::Equals( Impl::StdString::makeString( str ), caseSensitivity );
    }
    inline Impl::StdString::Contains    Contains( std::string const& substr, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes ) {
        return Impl::StdString::Contains( substr, caseSensitivity );
    }
    inline Impl::StdString::Contains    Contains( const char* substr, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes ) {
        return Impl::StdString::Contains( Impl::StdString::makeString( substr ), caseSensitivity );
    }
    inline Impl::StdString::StartsWith  StartsWith( std::string const& substr ) {
        return Impl::StdString::StartsWith( substr );
    }
    inline Impl::StdString::StartsWith  StartsWith( const char* substr ) {
        return Impl::StdString::StartsWith( Impl::StdString::makeString( substr ) );
    }
    inline Impl::StdString::EndsWith    EndsWith( std::string const& substr ) {
        return Impl::StdString::EndsWith( substr );
    }
    inline Impl::StdString::EndsWith    EndsWith( const char* substr ) {
        return Impl::StdString::EndsWith( Impl::StdString::makeString( substr ) );
    }

} // namespace Matchers

using namespace Matchers;

} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED
