/*
 *  Created by Phil Nash on 1/2/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MESSAGE_H_INCLUDED
#define TWOBLUECUBES_CATCH_MESSAGE_H_INCLUDED

#include "catch_result_type.h"
#include "catch_common.h"
#include "catch_stream.h"
#include "catch_interfaces_capture.h"
#include "catch_tostring.h"

#include <string>
#include <vector>

namespace Catch {

    struct MessageInfo {
        MessageInfo(    StringRef const& _macroName,
                        SourceLineInfo const& _lineInfo,
                        ResultWas::OfType _type );

        StringRef macroName;
        std::string message;
        SourceLineInfo lineInfo;
        ResultWas::OfType type;
        unsigned int sequence;

        bool operator == ( MessageInfo const& other ) const;
        bool operator < ( MessageInfo const& other ) const;
    private:
        static unsigned int globalCount;
    };

    struct MessageStream {

        template<typename T>
        MessageStream& operator << ( T const& value ) {
            m_stream << value;
            return *this;
        }

        ReusableStringStream m_stream;
    };

    struct MessageBuilder : MessageStream {
        MessageBuilder( StringRef const& macroName,
                        SourceLineInfo const& lineInfo,
                        ResultWas::OfType type );

        template<typename T>
        MessageBuilder& operator << ( T const& value ) {
            m_stream << value;
            return *this;
        }

        MessageInfo m_info;
    };

    class ScopedMessage {
    public:
        explicit ScopedMessage( MessageBuilder const& builder );
        ~ScopedMessage();

        MessageInfo m_info;
    };

    class Capturer {
        std::vector<MessageInfo> m_messages;
        IResultCapture& m_resultCapture = getResultCapture();
        size_t m_captured = 0;
    public:
        Capturer( StringRef macroName, SourceLineInfo const& lineInfo, ResultWas::OfType resultType, StringRef names );
        ~Capturer();

        void captureValue( size_t index, StringRef value );

        template<typename T>
        void captureValues( size_t index, T&& value ) {
            captureValue( index, Catch::Detail::stringify( value ) );
        }

        template<typename T, typename... Ts>
        void captureValues( size_t index, T&& value, Ts&&... values ) {
            captureValues( index, value );
            captureValues( index+1, values... );
        }
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_MESSAGE_H_INCLUDED
