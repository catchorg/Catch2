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

namespace Catch {

    MessageInfo::MessageInfo(   std::string const& _macroName,
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

    Catch::MessageBuilder::MessageBuilder( std::string const& macroName,
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
} // end namespace Catch
