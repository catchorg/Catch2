#include "catch_stringref.h"

#include <cstring>

namespace Catch {
    
    StringRef StringRef::s_emptyStringRef = "";
    
    StringRef::StringRef()
    :   StringRef( s_emptyStringRef )
    {}
    
    StringRef::StringRef( StringRef const& other )
    :   m_data( other.m_data ),
    m_size( other.m_size ),
    m_ownership( other.m_ownership )
    {
        if( m_ownership == Ownership::FullStringOwned )
            m_ownership = Ownership::FullStringRef;
    }
    
    StringRef::StringRef( StringRef&& other )
    :   m_data( other.m_data ),
    m_size( other.m_size ),
    m_ownership( other.m_ownership )
    {
        if( m_ownership == Ownership::FullStringOwned )
            other.m_ownership = Ownership::FullStringRef;
    }
    
    StringRef::StringRef( char const* rawChars )
    :   m_data( rawChars ),
    m_size( std::strlen( rawChars ) ),
    m_ownership( Ownership::FullStringRef )
    {}
    StringRef::StringRef( char const* rawChars, size_type size )
    :   m_data( rawChars ),
    m_size( size ),
    m_ownership( Ownership::SubStringRef )
    {
        size_type rawSize = std::strlen( rawChars );
        if( rawSize < size )
            size = rawSize;
        if( rawSize == size )
            m_ownership = Ownership::FullStringRef;
    }
    
    StringRef::~StringRef() {
        deleteIfOwned();
    }
    
    void StringRef::deleteIfOwned() {
        if( m_ownership == Ownership::FullStringOwned )
            delete m_data;
    }
    
    auto StringRef::operator = ( StringRef const& other ) -> StringRef& {
        if( &other == this )
            return *this;
        deleteIfOwned();
        m_data = other.m_data;
        m_size = other.m_size;
        if( other.m_ownership == Ownership::FullStringOwned )
            m_ownership = Ownership::FullStringRef;
            else
                m_ownership = other.m_ownership;
                return *this;
    }
    
    auto StringRef::operator = ( StringRef&& other ) -> StringRef& {
        if( &other == this )
            return *this;
        deleteIfOwned();
        m_data = other.m_data;
        m_size = other.m_size;
        m_ownership = other.m_ownership;
        if( other.m_ownership == Ownership::FullStringOwned )
            other.m_ownership = Ownership::FullStringRef;
            return *this;
    }
    
    auto StringRef::c_str() const -> char const* {
        if( m_ownership == Ownership::SubStringRef )
            const_cast<StringRef*>( this )->takeOwnership();
            return m_data;
    }
    
    void StringRef::takeOwnership() {
        if( m_ownership != Ownership::FullStringOwned ) {
            char* data = new char[m_size+1];
            std::memcpy( data, m_data, m_size );
            data[m_size] = '\0';
            m_data = data;
            m_ownership = Ownership::FullStringOwned;
        }
    }
    auto StringRef::substr( size_type start, size_type size ) const -> StringRef {
        if( start < m_size )
            return StringRef( m_data+start, size );
            else
                return StringRef();
                }
    auto StringRef::operator == ( StringRef const& other ) const -> bool {
        return size() == other.size() &&
        (std::strncmp( m_data, other.m_data, size() ) == 0);
    }
    auto StringRef::operator != ( StringRef const& other ) const -> bool {
        return !operator==( other );
    }
        

} // namespace Catch
