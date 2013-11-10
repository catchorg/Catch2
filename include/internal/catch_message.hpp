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

    template <typename T>
    struct MessageInfoCounter {
        // This may need protecting if threading support is added
        static T globalCount;
    };
    template <typename T>
    T MessageInfoCounter<T>::globalCount = T();

    INTERNAL_CATCH_INLINE MessageInfo::MessageInfo(   std::string const& _macroName,
                                SourceLineInfo const& _lineInfo,
                                ResultWas::OfType _type )
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        type( _type ),
        sequence( ++MessageInfoCounter<unsigned int>::globalCount )
    {}

    ////////////////////////////////////////////////////////////////////////////

    INTERNAL_CATCH_INLINE ScopedMessage::ScopedMessage( MessageBuilder const& builder )
    : m_info( builder.m_info )
    {
        m_info.message = builder.m_stream.str();
        getResultCapture().pushScopedMessage( m_info );
    }
    INTERNAL_CATCH_INLINE ScopedMessage::~ScopedMessage() {
        getResultCapture().popScopedMessage( m_info );
    }


} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_MESSAGE_HPP_INCLUDED
