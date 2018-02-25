/*
 *  Created by Phil Nash on 1/2/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MESSAGE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_MESSAGE_HPP_INCLUDED

#include "catch_message.h"

namespace Catch {

    MessageInfo::MessageInfo(   std::string const& _macroName,
                                SourceLineInfo const& _lineInfo,
                                ResultWas::OfType _type )
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        type( _type ),
        sequence( ++globalCount )
    {}

    // This may need protecting if threading support is added
    unsigned int MessageInfo::globalCount = 0;


    ////////////////////////////////////////////////////////////////////////////

    ScopedMessage::ScopedMessage( MessageBuilder const& builder )
    : m_info( builder.m_info )
    {
        m_info.message = builder.m_stream.str();
        getResultCapture().pushScopedMessage( m_info );
    }
    ScopedMessage::ScopedMessage( ScopedMessage const& other )
    : m_info( other.m_info )
    {}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4996) // std::uncaught_exception is deprecated in C++17
#endif
    ScopedMessage::~ScopedMessage() {
        if ( !std::uncaught_exception() ){
            getResultCapture().popScopedMessage(m_info);
        }
    }
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_MESSAGE_HPP_INCLUDED
