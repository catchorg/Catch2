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

    template<typename ExpressionT>
    struct Matcher : SharedImpl<IShared>
    {
        typedef ExpressionT ExpressionType;

        virtual ~Matcher() {}
        virtual Ptr<Matcher> clone() const = 0;
        virtual bool match( const ExpressionT& expr ) const = 0;
        virtual std::string toString() const = 0;
    };

    template<typename DerivedT, typename ExpressionT>
    struct MatcherImpl : Matcher<ExpressionT> {

        virtual Ptr<Matcher<ExpressionT> > clone() const {
            return Ptr<Matcher<ExpressionT> >( new DerivedT( static_cast<const DerivedT&>( *this ) ) );
        }
    };

    namespace Generic {

        template<typename ExpressionT>
        class AllOf : public MatcherImpl<AllOf<ExpressionT>, ExpressionT> {
        public:

            AllOf() {}
            AllOf( const AllOf& other ) : m_matchers( other.m_matchers ) {}

            AllOf& add( const Matcher<ExpressionT>& matcher ) {
                m_matchers.push_back( matcher.clone() );
                return *this;
            }
            virtual bool match( const ExpressionT& expr ) const
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

        private:
            std::vector<Ptr<Matcher<ExpressionT> > > m_matchers;
        };
        
        template<typename ExpressionT>
        class AnyOf : public MatcherImpl<AnyOf<ExpressionT>, ExpressionT> {
        public:

            AnyOf() {}
            AnyOf( const AnyOf& other ) : m_matchers( other.m_matchers ) {}

            AnyOf& add( const Matcher<ExpressionT>& matcher ) {
                m_matchers.push_back( matcher.clone() );
                return *this;
            }
            virtual bool match( const ExpressionT& expr ) const
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

        private:
            std::vector<Ptr<Matcher<ExpressionT> > > m_matchers;
        };
        
    }

    namespace StdString {

        struct Equals : MatcherImpl<Equals, std::string> {
            Equals( const std::string& str ) : m_str( str ){}
            Equals( const Equals& other ) : m_str( other.m_str ){}

            virtual ~Equals();

            virtual bool match( const std::string& expr ) const {
                return m_str == expr;
            }
            virtual std::string toString() const {
                return "equals: \"" + m_str + "\"";
            }

            std::string m_str;
        };
        
        struct Contains : MatcherImpl<Contains, std::string> {
            Contains( const std::string& substr ) : m_substr( substr ){}
            Contains( const Contains& other ) : m_substr( other.m_substr ){}

            virtual ~Contains();

            virtual bool match( const std::string& expr ) const {
                return expr.find( m_substr ) != std::string::npos;
            }
            virtual std::string toString() const {
                return "contains: \"" + m_substr + "\"";
            }

            std::string m_substr;
        };
        
        struct StartsWith : MatcherImpl<StartsWith, std::string> {
            StartsWith( const std::string& substr ) : m_substr( substr ){}
            StartsWith( const StartsWith& other ) : m_substr( other.m_substr ){}

            virtual ~StartsWith();

            virtual bool match( const std::string& expr ) const {
                return expr.find( m_substr ) == 0;
            }
            virtual std::string toString() const {
                return "starts with: \"" + m_substr + "\"";
            }

            std::string m_substr;
        };
        
        struct EndsWith : MatcherImpl<EndsWith, std::string> {
            EndsWith( const std::string& substr ) : m_substr( substr ){}
            EndsWith( const EndsWith& other ) : m_substr( other.m_substr ){}

            virtual ~EndsWith();

            virtual bool match( const std::string& expr ) const {
                return expr.find( m_substr ) == expr.size() - m_substr.size();
            }
            virtual std::string toString() const {
                return "ends with: \"" + m_substr + "\"";
            }

            std::string m_substr;
        };
    } // namespace StdString
    } // namespace Impl

    // The following functions create the actual matcher objects.
    // This allows the types to be inferred
    template<typename ExpressionT>
    inline Impl::Generic::AllOf<ExpressionT> AllOf( const Impl::Matcher<ExpressionT>& m1,
                                                    const Impl::Matcher<ExpressionT>& m2 ) {
        return Impl::Generic::AllOf<ExpressionT>().add( m1 ).add( m2 );
    }
    template<typename ExpressionT>
    inline Impl::Generic::AllOf<ExpressionT> AllOf( const Impl::Matcher<ExpressionT>& m1,
                                                    const Impl::Matcher<ExpressionT>& m2,
                                                    const Impl::Matcher<ExpressionT>& m3 ) {
        return Impl::Generic::AllOf<ExpressionT>().add( m1 ).add( m2 ).add( m3 );
    }
    template<typename ExpressionT>
    inline Impl::Generic::AnyOf<ExpressionT> AnyOf( const Impl::Matcher<ExpressionT>& m1,
                                                    const Impl::Matcher<ExpressionT>& m2 ) {
        return Impl::Generic::AnyOf<ExpressionT>().add( m1 ).add( m2 );
    }
    template<typename ExpressionT>
    inline Impl::Generic::AnyOf<ExpressionT> AnyOf( const Impl::Matcher<ExpressionT>& m1,
                                                    const Impl::Matcher<ExpressionT>& m2,
                                                    const Impl::Matcher<ExpressionT>& m3 ) {
        return Impl::Generic::AnyOf<ExpressionT>().add( m1 ).add( m2 ).add( m3 );
    }

    inline Impl::StdString::Equals      Equals( const std::string& str ){ return Impl::StdString::Equals( str ); }
    inline Impl::StdString::Contains    Contains( const std::string& substr ){ return Impl::StdString::Contains( substr ); }
    inline Impl::StdString::StartsWith  StartsWith( const std::string& substr ){ return Impl::StdString::StartsWith( substr ); }
    inline Impl::StdString::EndsWith    EndsWith( const std::string& substr ){ return Impl::StdString::EndsWith( substr ); }
    
} // namespace Matchers
    
using namespace Matchers;
    
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED
