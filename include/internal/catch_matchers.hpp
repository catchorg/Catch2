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
    namespace StdString {

        struct Equals {
            Equals( const std::string& str ) : m_str( str ){}
            
            bool operator()( const std::string& str ) const
            {
                return str == m_str;
            }
            
            friend std::ostream& operator<<( std::ostream& os, const Equals& matcher )
            {
                os << "equals: \"" << matcher.m_str << "\"";
                return os;
            }
            std::string m_str;
        };
        
        struct Contains {
            Contains( const std::string& substr ) : m_substr( substr ){}
            
            bool operator()( const std::string& str ) const
            {
                return str.find( m_substr ) != std::string::npos;
            }
            
            friend std::ostream& operator<<( std::ostream& os, const Contains& matcher )
            {
                os << "contains: \"" << matcher.m_substr << "\"";
                return os;
            }
            std::string m_substr;
        };
        
        struct StartsWith {
            StartsWith( const std::string& substr ) : m_substr( substr ){}
            
            bool operator()( const std::string& str ) const
            {
                return str.find( m_substr ) == 0;
            }
            
            friend std::ostream& operator<<( std::ostream& os, const StartsWith& matcher )
            {
                os << "starts with: \"" << matcher.m_substr << "\"";
                return os;
            }
            std::string m_substr;
        };
        
        struct EndsWith {
            EndsWith( const std::string& substr ) : m_substr( substr ){}
            
            bool operator()( const std::string& str ) const
            {
                return str.find( m_substr ) == str.size() - m_substr.size();
            }
            
            friend std::ostream& operator<<( std::ostream& os, const EndsWith& matcher )
            {
                os << "ends with: \"" << matcher.m_substr << "\"";
                return os;
            }
            std::string m_substr;
        };
    } // namespace StdString
    } // namespace Impl
    
    inline Impl::StdString::Equals      Equals( const std::string& str ){ return Impl::StdString::Equals( str ); }
    inline Impl::StdString::Contains    Contains( const std::string& substr ){ return Impl::StdString::Contains( substr ); }
    inline Impl::StdString::StartsWith  StartsWith( const std::string& substr ){ return Impl::StdString::StartsWith( substr ); }
    inline Impl::StdString::EndsWith    EndsWith( const std::string& substr ){ return Impl::StdString::EndsWith( substr ); }
    
} // namespace Matchers
    
using namespace Matchers;
    
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED
