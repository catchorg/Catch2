/*
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_COMMON_H_INCLUDED
#define TWOBLUECUBES_CATCH_COMMON_H_INCLUDED

#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) name##line
#define INTERNAL_CATCH_UNIQUE_NAME_LINE( name, line ) INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line )
#define INTERNAL_CATCH_UNIQUE_NAME( name ) INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __LINE__ )

#define INTERNAL_CATCH_STRINGIFY2( expr ) #expr
#define INTERNAL_CATCH_STRINGIFY( expr ) INTERNAL_CATCH_STRINGIFY2( expr )

#ifdef __GNUC__
#define CATCH_ATTRIBUTE_NORETURN __attribute__ ((noreturn))
#else
#define CATCH_ATTRIBUTE_NORETURN
#endif

#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace Catch {

	class NonCopyable {
		NonCopyable( const NonCopyable& );
		void operator = ( const NonCopyable& );
	protected:
		NonCopyable() {}
		virtual ~NonCopyable();
	};
    
    class SafeBool {
    public:
        typedef void (SafeBool::*type)() const;

        static type makeSafe( bool value ) {
            return value ? &SafeBool::trueValue : 0;
        }
    private:
        void trueValue() const {}
    };
  
    template<typename ContainerT>
    inline void deleteAll( ContainerT& container ) {
        typename ContainerT::const_iterator it = container.begin();
        typename ContainerT::const_iterator itEnd = container.end();
        for(; it != itEnd; ++it )
        {
            delete *it;
        }
    }
    template<typename AssociativeContainerT>
    inline void deleteAllValues( AssociativeContainerT& container ) {
        typename AssociativeContainerT::const_iterator it = container.begin();
        typename AssociativeContainerT::const_iterator itEnd = container.end();
        for(; it != itEnd; ++it )
        {
            delete it->second;
        }
    }
    
    template<typename ContainerT, typename Function>
    inline void forEach( ContainerT& container, Function function ) {
        std::for_each( container.begin(), container.end(), function );
    }
    
    template<typename ContainerT, typename Function>
    inline void forEach( const ContainerT& container, Function function ) {
        std::for_each( container.begin(), container.end(), function );
    }

    inline bool startsWith( const std::string& s, const std::string& prefix ) {
        return s.size() >= prefix.size() && s.substr( 0, prefix.size() ) == prefix;
    }
    inline bool endsWith( const std::string& s, const std::string& suffix ) {
        return s.size() >= suffix.size() && s.substr( s.size()-suffix.size(), suffix.size() ) == suffix;
    }
    inline bool contains( const std::string& s, const std::string& infix ) {
        return s.find( infix ) != std::string::npos;
    }

    struct pluralise {
        pluralise( std::size_t count, const std::string& label )
        :   m_count( count ),
            m_label( label )
        {}

        friend std::ostream& operator << ( std::ostream& os, const pluralise& pluraliser ) {
            os << pluraliser.m_count << " " << pluraliser.m_label;
            if( pluraliser.m_count != 1 )
            os << "s";
            return os;
        }

        std::size_t m_count;
        std::string m_label;
    };

    struct SourceLineInfo {
    
        SourceLineInfo() : line( 0 ){}
        SourceLineInfo( const std::string& _file, std::size_t _line )
        :   file( _file ),
            line( _line )
        {}
        SourceLineInfo( const std::string& _function, const std::string& _file, std::size_t _line )
        :   function( _function ),
            file( _file ),
            line( _line )
        {}
        SourceLineInfo( const SourceLineInfo& other )
        :   file( other.file ),
            line( other.line )
        {}
        void swap( SourceLineInfo& other ){
            file.swap( other.file );
            std::swap( line, other.line );
        }
        
        std::string function;
        std::string file;
        std::size_t line;        
    };
    
    inline std::ostream& operator << ( std::ostream& os, const SourceLineInfo& info ) {
#ifndef __GNUG__
        os << info.file << "(" << info.line << "): ";
#else                
        os << info.file << ":" << info.line << ": ";            
#endif            
        return os;
    }
    
    CATCH_ATTRIBUTE_NORETURN
    inline void throwLogicError( const std::string& message, const SourceLineInfo& locationInfo ) {
        std::ostringstream oss;
        oss << "Internal Catch error: '" << message << "' at: " << locationInfo;
        throw std::logic_error( oss.str() );
    }
}

#define CATCH_INTERNAL_LINEINFO ::Catch::SourceLineInfo( __FILE__, static_cast<std::size_t>( __LINE__ ) )
#define CATCH_INTERNAL_ERROR( msg ) ::Catch::throwLogicError( msg, CATCH_INTERNAL_LINEINFO );

#endif // TWOBLUECUBES_CATCH_COMMON_H_INCLUDED

