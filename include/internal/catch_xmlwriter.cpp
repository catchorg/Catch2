/*
 *  Created by Phil on 19/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_xmlwriter.h"

#include <iomanip>
#include <type_traits>

namespace Catch {

namespace {

    bool shouldNewline(XmlFormatting fmt) {
        return !!(static_cast<std::underlying_type<XmlFormatting>::type>(fmt & XmlFormatting::Newline));
    }

    bool shouldIndent(XmlFormatting fmt) {
        return !!(static_cast<std::underlying_type<XmlFormatting>::type>(fmt & XmlFormatting::Indent));
    }

}

    XmlFormatting operator | (XmlFormatting lhs, XmlFormatting rhs) {
        return static_cast<XmlFormatting>(
            static_cast<std::underlying_type<XmlFormatting>::type>(lhs) |
            static_cast<std::underlying_type<XmlFormatting>::type>(rhs)
        );
    }

    XmlFormatting operator & (XmlFormatting lhs, XmlFormatting rhs) {
        return static_cast<XmlFormatting>(
            static_cast<std::underlying_type<XmlFormatting>::type>(lhs) &
            static_cast<std::underlying_type<XmlFormatting>::type>(rhs)
        );
    }



    XmlEncode::XmlEncode( std::string const& str, ForWhat forWhat )
    :   m_str( str ),
        m_forWhat( forWhat )
    {}

    void XmlEncode::encodeTo( std::ostream& os ) const {

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

    std::ostream& operator << ( std::ostream& os, XmlEncode const& xmlEncode ) {
        xmlEncode.encodeTo( os );
        return os;
    }

    XmlWriter::ScopedElement::ScopedElement( XmlWriter* writer, XmlFormatting fmt )
    :   m_writer( writer ),
        m_fmt( fmt )
    {}

    XmlWriter::ScopedElement::ScopedElement( ScopedElement&& other ) noexcept
    :   m_writer( other.m_writer ),
        m_fmt( other.m_fmt ){
        other.m_writer = nullptr;
        other.m_fmt = XmlFormatting::None;
    }
    XmlWriter::ScopedElement& XmlWriter::ScopedElement::operator=( ScopedElement&& other ) noexcept {
        if ( m_writer ) {
            m_writer->endElement();
        }
        m_writer = other.m_writer;
        m_fmt = other.m_fmt;
        other.m_writer = nullptr;
        other.m_fmt = XmlFormatting::None;
        return *this;
    }


    XmlWriter::ScopedElement::~ScopedElement() {
        if( m_writer )
            m_writer->endElement(m_fmt);
    }

    XmlWriter::ScopedElement& XmlWriter::ScopedElement::writeText( std::string const& text, XmlFormatting fmt ) {
        m_writer->writeText( text, fmt );
        return *this;
    }

    XmlWriter::XmlWriter( std::ostream& os ) : m_os( os )
    {
        writeDeclaration();
    }

    XmlWriter::~XmlWriter() {
        while( !m_tags.empty() )
            endElement();
        newlineIfNecessary();
    }

    XmlWriter& XmlWriter::startElement( std::string const& name, XmlFormatting fmt ) {
        ensureTagClosed();
        newlineIfNecessary();
        if (shouldIndent(fmt)) {
            m_os << m_indent;
            m_indent += "  ";
        }
        m_os << '<' << name;
        m_tags.push_back( name );
        m_tagIsOpen = true;
        applyFormatting(fmt);
        return *this;
    }

    XmlWriter::ScopedElement XmlWriter::scopedElement( std::string const& name, XmlFormatting fmt ) {
        ScopedElement scoped( this, fmt );
        startElement( name, fmt );
        return scoped;
    }

    XmlWriter& XmlWriter::endElement( XmlFormatting fmt ) {
        m_indent = m_indent.substr(0, m_indent.size() - 2);
        if ( m_tagIsOpen ) {
            m_os << "/>";
            m_tagIsOpen = false;
        } else {
            newlineIfNecessary();
            if (shouldIndent(fmt)) {
                m_os << m_indent;
            }
            m_os << "</" << m_tags.back() << ">";
        }
        m_os << std::flush;
        applyFormatting(fmt);
        m_tags.pop_back();
        return *this;
    }

    XmlWriter& XmlWriter::writeAttribute( std::string const& name, std::string const& attribute ) {
        if( !name.empty() && !attribute.empty() )
            m_os << ' ' << name << "=\"" << XmlEncode( attribute, XmlEncode::ForAttributes ) << '"';
        return *this;
    }

    XmlWriter& XmlWriter::writeAttribute( std::string const& name, bool attribute ) {
        m_os << ' ' << name << "=\"" << ( attribute ? "true" : "false" ) << '"';
        return *this;
    }

    XmlWriter& XmlWriter::writeText( std::string const& text, XmlFormatting fmt ) {
        if( !text.empty() ){
            bool tagWasOpen = m_tagIsOpen;
            ensureTagClosed();
            if( tagWasOpen && shouldIndent(fmt) )
                m_os << m_indent;
            m_os << XmlEncode( text );
            applyFormatting(fmt);
        }
        return *this;
    }

    XmlWriter& XmlWriter::writeComment( std::string const& text, XmlFormatting fmt ) {
        ensureTagClosed();
        if (shouldIndent(fmt)) {
            m_os << m_indent;
        }
        m_os << "<!--" << text << "-->";
        applyFormatting(fmt);
        return *this;
    }

    void XmlWriter::writeStylesheetRef( std::string const& url ) {
        m_os << "<?xml-stylesheet type=\"text/xsl\" href=\"" << url << "\"?>\n";
    }

    XmlWriter& XmlWriter::writeBlankLine() {
        ensureTagClosed();
        m_os << '\n';
        return *this;
    }

    void XmlWriter::ensureTagClosed() {
        if( m_tagIsOpen ) {
            m_os << '>' << std::flush;
            m_tagIsOpen = false;
        }
    }

    void XmlWriter::applyFormatting(XmlFormatting fmt) {
        m_needsNewline = shouldNewline(fmt);
    }

    void XmlWriter::writeDeclaration() {
        m_os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    }

    void XmlWriter::newlineIfNecessary() {
        if( m_needsNewline ) {
            m_os << std::endl;
            m_needsNewline = false;
        }
    }
}
