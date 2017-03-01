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
                        // Escape control chars - based on contribution by @espenalb in PR #465 and
                        // by @mrpi PR #588
                        if ( ( c >= 0 && c < '\x09' ) || ( c > '\x0D' && c < '\x20') || c=='\x7F' ) {
                            // see http://stackoverflow.com/questions/404107/why-are-control-characters-illegal-in-xml-1-0
                            os << "\\x" << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
                               << static_cast<int>( c );
                        }
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
            m_os( Catch::cout() )
        {
            writeDeclaration();
        }

        XmlWriter( std::ostream& os )
        :   m_tagIsOpen( false ),
            m_needsNewline( false ),
            m_os( os )
        {
            writeDeclaration();
        }

        ~XmlWriter() {
            while( !m_tags.empty() )
                endElement();
        }

        XmlWriter& startElement( std::string const& name ) {
            ensureTagClosed();
            newlineIfNecessary();
            m_os << m_indent << '<' << name;
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
                m_os << "/>";
                m_tagIsOpen = false;
            }
            else {
                m_os << m_indent << "</" << m_tags.back() << ">";
            }
            m_os << std::endl;
            m_tags.pop_back();
            return *this;
        }

        XmlWriter& writeAttribute( std::string const& name, std::string const& attribute ) {
            if( !name.empty() && !attribute.empty() )
                m_os << ' ' << name << "=\"" << XmlEncode( attribute, XmlEncode::ForAttributes ) << '"';
            return *this;
        }

        XmlWriter& writeAttribute( std::string const& name, bool attribute ) {
            m_os << ' ' << name << "=\"" << ( attribute ? "true" : "false" ) << '"';
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
                    m_os << m_indent;
                m_os << XmlEncode( text );
                m_needsNewline = true;
            }
            return *this;
        }

        XmlWriter& writeComment( std::string const& text ) {
            ensureTagClosed();
            m_os << m_indent << "<!--" << text << "-->";
            m_needsNewline = true;
            return *this;
        }

        void writeStylesheetRef( std::string const& url ) {
            m_os << "<?xml-stylesheet type=\"text/xsl\" href=\"" << url << "\"?>\n";
        }

        XmlWriter& writeBlankLine() {
            ensureTagClosed();
            m_os << '\n';
            return *this;
        }

        void ensureTagClosed() {
            if( m_tagIsOpen ) {
                m_os << ">" << std::endl;
                m_tagIsOpen = false;
            }
        }

    private:
        XmlWriter( XmlWriter const& );
        void operator=( XmlWriter const& );

        void writeDeclaration() {
            m_os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        }

        void newlineIfNecessary() {
            if( m_needsNewline ) {
                m_os << std::endl;
                m_needsNewline = false;
            }
        }

        bool m_tagIsOpen;
        bool m_needsNewline;
        std::vector<std::string> m_tags;
        std::string m_indent;
        std::ostream& m_os;
    };

}
#include "catch_reenable_warnings.h"

#endif // TWOBLUECUBES_CATCH_XMLWRITER_HPP_INCLUDED
