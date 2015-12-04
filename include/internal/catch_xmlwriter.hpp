/*
 *  Created by Phil on 09/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_XMLWRITER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_XMLWRITER_HPP_INCLUDED

#include "catch_stream.h"
#include "catch_compiler_capabilities.h"
#include "catch_suppress_warnings.h"

#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

namespace Catch {

    class XmlEncode {
    public:
        enum ForWhat { ForTextNodes, ForAttributes };

        XmlEncode( std::string const& str, ForWhat forWhat = ForTextNodes )
        :   m_str( str ),
            m_forWhat( forWhat )
        {}

        void encodeTo( std::ostream& os ) const {

            // Apostrophe escaping not necessary if we always use " to write attributes
            // (see: http://www.w3.org/TR/xml/#syntax)

            for( std::size_t i = 0; i < m_str.size(); ++ i ) {
                char c = m_str[i];
                switch( c ) {
                    case '<':   os << "&lt;"; break;
                    case '&':   os << "&amp;"; break;

                    case '>':
                        // See: http://www.w3.org/TR/xml/#syntax
                        if( i > 2 && m_str[i-1] == ']' && m_str[i-2] == ']' )
                            os << "&gt;";
                        else
                            os << c;
                        break;

                    case '\"':
                        if( m_forWhat == ForAttributes )
                            os << "&quot;";
                        else
                            os << c;
                        break;

                    default:
                        // Escape control chars - based on contribution by @espenalb in PR #465
                        if ( ( c < '\x09' ) || ( c > '\x0D' && c < '\x20') || c=='\x7F' )
                            os << "&#x" << std::uppercase << std::hex << static_cast<int>( c );
                        else
                            os << c;
                }
            }
        }

        friend std::ostream& operator << ( std::ostream& os, XmlEncode const& xmlEncode ) {
            xmlEncode.encodeTo( os );
            return os;
        }

    private:
        std::string m_str;
        ForWhat m_forWhat;
    };

    class XmlWriter {
    public:

        class ScopedElement {
        public:
            ScopedElement( XmlWriter* writer )
            :   m_writer( writer )
            {}

            ScopedElement( ScopedElement const& other )
            :   m_writer( other.m_writer ){
                other.m_writer = CATCH_NULL;
            }

            ~ScopedElement() {
                if( m_writer )
                    m_writer->endElement();
            }

            ScopedElement& writeText( std::string const& text, bool indent = true ) {
                m_writer->writeText( text, indent );
                return *this;
            }

            template<typename T>
            ScopedElement& writeAttribute( std::string const& name, T const& attribute ) {
                m_writer->writeAttribute( name, attribute );
                return *this;
            }

        private:
            mutable XmlWriter* m_writer;
        };

        XmlWriter()
        :   m_tagIsOpen( false ),
            m_needsNewline( false ),
            m_os( &Catch::cout() )
        {}

        XmlWriter( std::ostream& os )
        :   m_tagIsOpen( false ),
            m_needsNewline( false ),
            m_os( &os )
        {}

        ~XmlWriter() {
            while( !m_tags.empty() )
                endElement();
        }

        XmlWriter& startElement( std::string const& name ) {
            ensureTagClosed();
            newlineIfNecessary();
            stream() << m_indent << "<" << name;
            m_tags.push_back( name );
            m_indent += "  ";
            m_tagIsOpen = true;
            return *this;
        }

        ScopedElement scopedElement( std::string const& name ) {
            ScopedElement scoped( this );
            startElement( name );
            return scoped;
        }

        XmlWriter& endElement() {
            newlineIfNecessary();
            m_indent = m_indent.substr( 0, m_indent.size()-2 );
            if( m_tagIsOpen ) {
                stream() << "/>\n";
                m_tagIsOpen = false;
            }
            else {
                stream() << m_indent << "</" << m_tags.back() << ">\n";
            }
            m_tags.pop_back();
            return *this;
        }

        XmlWriter& writeAttribute( std::string const& name, std::string const& attribute ) {
            if( !name.empty() && !attribute.empty() )
                stream() << " " << name << "=\"" << XmlEncode( attribute, XmlEncode::ForAttributes ) << "\"";
            return *this;
        }

        XmlWriter& writeAttribute( std::string const& name, bool attribute ) {
            stream() << " " << name << "=\"" << ( attribute ? "true" : "false" ) << "\"";
            return *this;
        }

        template<typename T>
        XmlWriter& writeAttribute( std::string const& name, T const& attribute ) {
            std::ostringstream oss;
            oss << attribute;
            return writeAttribute( name, oss.str() );
        }

        XmlWriter& writeText( std::string const& text, bool indent = true ) {
            if( !text.empty() ){
                bool tagWasOpen = m_tagIsOpen;
                ensureTagClosed();
                if( tagWasOpen && indent )
                    stream() << m_indent;
                stream() << XmlEncode( text );
                m_needsNewline = true;
            }
            return *this;
        }

        XmlWriter& writeComment( std::string const& text ) {
            ensureTagClosed();
            stream() << m_indent << "<!--" << text << "-->";
            m_needsNewline = true;
            return *this;
        }

        XmlWriter& writeBlankLine() {
            ensureTagClosed();
            stream() << "\n";
            return *this;
        }

        void setStream( std::ostream& os ) {
            m_os = &os;
        }

    private:
        XmlWriter( XmlWriter const& );
        void operator=( XmlWriter const& );

        std::ostream& stream() {
            return *m_os;
        }

        void ensureTagClosed() {
            if( m_tagIsOpen ) {
                stream() << ">\n";
                m_tagIsOpen = false;
            }
        }

        void newlineIfNecessary() {
            if( m_needsNewline ) {
                stream() << "\n";
                m_needsNewline = false;
            }
        }

        bool m_tagIsOpen;
        bool m_needsNewline;
        std::vector<std::string> m_tags;
        std::string m_indent;
        std::ostream* m_os;
    };

}
#include "catch_reenable_warnings.h"

#endif // TWOBLUECUBES_CATCH_XMLWRITER_HPP_INCLUDED
