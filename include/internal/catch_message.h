/*
 *  Created by Phil Nash on 1/2/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MESSAGE_H_INCLUDED
#define TWOBLUECUBES_CATCH_MESSAGE_H_INCLUDED

#include <string>
#include "catch_result_type.h"
#include "catch_common.h"

namespace Catch {
    
    struct MessageInfo {
        MessageInfo(    std::string const& _macroName,
                        SourceLineInfo const& _lineInfo,
                        ResultWas::OfType _type );

        std::string macroName;
        SourceLineInfo lineInfo;
        ResultWas::OfType type;
        std::string message;
        unsigned int sequence;
        
        bool operator == ( MessageInfo const& other ) const {
            return sequence == other.sequence;
        }
        bool operator < ( MessageInfo const& other ) const {
            return sequence < other.sequence;
        }
    private:
        static unsigned int globalCount;
    };
    
    
    class MessageBuilder : public MessageInfo {
    public:
        MessageBuilder( std::string const& _macroName,
                       SourceLineInfo const& _lineInfo,
                       ResultWas::OfType _type );
        
        MessageInfo build() const;
        
        template<typename T>
        MessageBuilder& operator << ( T const& _value ) {
            stream << _value;
            return *this;
        }
    private:
        std::ostringstream stream;
    };
    
    class ScopedMessageBuilder : public MessageBuilder {
    public:
        ScopedMessageBuilder(   std::string const& _macroName,
                                SourceLineInfo const& _lineInfo,
                                ResultWas::OfType _type );
        ~ScopedMessageBuilder();
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_MESSAGE_H_INCLUDED
