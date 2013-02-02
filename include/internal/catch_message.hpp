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
    
    MessageBuilder::MessageBuilder( std::string const& _macroName,
                                    SourceLineInfo const& _lineInfo,
                                    ResultWas::OfType _type )
    :   MessageInfo( _macroName, _lineInfo, _type )
    {}
    
    MessageInfo MessageBuilder::build() const {
        MessageInfo message = *this;
        message.message = stream.str();
        return message;
    }

    ////////////////////////////////////////////////////////////////////////////
    
    ScopedMessageBuilder::ScopedMessageBuilder
        (   std::string const& _macroName,
            SourceLineInfo const& _lineInfo,
            ResultWas::OfType _type )
    : MessageBuilder( _macroName, _lineInfo, _type )
    {}
    
    ScopedMessageBuilder::~ScopedMessageBuilder() {
        getResultCapture().popScopedMessage( *this );
    }
    
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_MESSAGE_HPP_INCLUDED
