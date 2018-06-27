/*
 *  Created by Phil Nash on 1/2/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_message.h"
#include "catch_interfaces_capture.h"
#include "catch_uncaught_exceptions.h"

#include <cassert>

namespace Catch {

    MessageInfo::MessageInfo(   StringRef const& _macroName,
                                SourceLineInfo const& _lineInfo,
                                ResultWas::OfType _type )
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        type( _type ),
        sequence( ++globalCount )
    {}

    bool MessageInfo::operator==( MessageInfo const& other ) const {
        return sequence == other.sequence;
    }

    bool MessageInfo::operator<( MessageInfo const& other ) const {
        return sequence < other.sequence;
    }

    // This may need protecting if threading support is added
    unsigned int MessageInfo::globalCount = 0;


    ////////////////////////////////////////////////////////////////////////////

    Catch::MessageBuilder::MessageBuilder( StringRef const& macroName,
                                           SourceLineInfo const& lineInfo,
                                           ResultWas::OfType type )
        :m_info(macroName, lineInfo, type) {}

    ////////////////////////////////////////////////////////////////////////////


    ScopedMessage::ScopedMessage( MessageBuilder const& builder )
    : m_info( builder.m_info )
    {
        m_info.message = builder.m_stream.str();
        getResultCapture().pushScopedMessage( m_info );
    }

    ScopedMessage::~ScopedMessage() {
        if ( !uncaught_exceptions() ){
            getResultCapture().popScopedMessage(m_info);
        }
    }


    Capturer::Capturer( StringRef macroName, SourceLineInfo const& lineInfo, ResultWas::OfType resultType, StringRef names ) {
        auto start = std::string::npos;
        for( size_t pos = 0; pos <= names.size(); ++pos ) {
            char c = names[pos];
            if( pos == names.size() || c == ' ' || c == '\t' || c == ',' || c == ']' ) {
                if( start != std::string::npos ) {
                    m_messages.push_back( MessageInfo( macroName, lineInfo, resultType ) );
                    m_messages.back().message = names.substr( start, pos-start) + " := ";
                    start = std::string::npos;
                }
            }
            else if( c != '[' && c != ']' && start == std::string::npos )
                start = pos;
        }
    }
    Capturer::~Capturer() {
        if ( !uncaught_exceptions() ){
            assert( m_captured == m_messages.size() );
            for( size_t i = 0; i < m_captured; ++i  )
                m_resultCapture.popScopedMessage( m_messages[i] );
        }
    }

    void Capturer::captureValue( size_t index, StringRef value ) {
        assert( index < m_messages.size() );
        m_messages[index].message += value;
        m_resultCapture.pushScopedMessage( m_messages[index] );
        m_captured++;
    }

} // end namespace Catch
