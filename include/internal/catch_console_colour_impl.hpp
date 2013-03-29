/*
 *  Created by Phil on 25/2/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONSOLE_COLOUR_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONSOLE_COLOUR_IMPL_HPP_INCLUDED

#include "catch_console_colour.hpp"

#if defined ( CATCH_PLATFORM_WINDOWS ) /////////////////////////////////////////

#include <windows.h>

namespace {
    using namespace Catch;
    
    WORD mapConsoleColour( IConsoleColourCodes::Colours colour ) {
        enum Win32Colours {
                Grey = FOREGROUND_INTENSITY,
                BrightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
                BrightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
                DarkGreen = FOREGROUND_GREEN,
                Turquoise = FOREGROUND_BLUE | FOREGROUND_GREEN,
                Yellow = FOREGROUND_RED | FOREGROUND_GREEN
        };
        switch( colour ) {
            case IConsoleColourCodes::FileName:             return Grey;
            case IConsoleColourCodes::ResultError:          return BrightRed;
            case IConsoleColourCodes::ResultSuccess:        return BrightGreen;
            case IConsoleColourCodes::Error:                return BrightRed;
            case IConsoleColourCodes::Success:              return DarkGreen;
            case IConsoleColourCodes::OriginalExpression:   return Turquoise;
            case IConsoleColourCodes::ReconstructedExpression: return Yellow;
            case IConsoleColourCodes::SecondaryText:        return Grey;
            default: return 0;
        }
    }

    struct WindowsConsoleColourCodes : IConsoleColourCodes {
    
        WindowsConsoleColourCodes()
        :   hStdout( GetStdHandle(STD_OUTPUT_HANDLE) ),
            wOldColorAttrs( 0 )
        {
            GetConsoleScreenBufferInfo( hStdout, &csbiInfo );
            wOldColorAttrs = csbiInfo.wAttributes;
        }
        
        ~WindowsConsoleColourCodes() {
            SetConsoleTextAttribute( hStdout, wOldColorAttrs );
        }
        
        void set( Colours colour ) {
            WORD consoleColour = mapConsoleColour( colour );
            if( consoleColour > 0 )
                SetConsoleTextAttribute( hStdout, consoleColour );
        }
        
        HANDLE hStdout;
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
        WORD wOldColorAttrs;
    };

    inline bool shouldUseColourForPlatform() {
        return true;
    }
    
    typedef WindowsConsoleColourCodes PlatformConsoleColourCodes;

} // end anon namespace

#else // Not Windows - assumed to be POSIX compatible //////////////////////////

#include <unistd.h>

namespace {
    using namespace Catch;

    // use POSIX/ ANSI console terminal codes
    // Implementation contributed by Adam Strzelecki (http://github.com/nanoant)
    // https://github.com/philsquared/Catch/pull/131

    const char* WhiteOrNormal = "[0m";
    const char* BoldRed =       "[1;31m";
    const char* BoldGreen =     "[1;32m";
    const char* Green =         "[0;32m";
    const char* Cyan =          "[0;36m";
    const char* Yellow =        "[0;33m";
    const char* LightGrey =     "[0;37m";
//    const char* DarkGrey =      "[1;30m";

    struct AnsiConsoleColourCodes : IConsoleColourCodes {
    
        ~AnsiConsoleColourCodes() {
            set( None );
        }

        const char* escapeCodeForColour( Colours colour ) {
            switch( colour ) {
                case FileName:              return WhiteOrNormal;
                case ResultError:           return BoldRed;
                case ResultSuccess:         return BoldGreen;
                case Error:                 return BoldRed;
                case Success:               return Green;
                case OriginalExpression:    return Cyan;
                case ReconstructedExpression: return Yellow;
                case SecondaryText:         return LightGrey;
                case None:                  return WhiteOrNormal;
                }
        }

        void set( Colours colour ) {
            std::cout << '\033' << escapeCodeForColour( colour );
        }
    };

    inline bool shouldUseColourForPlatform() {
        return isatty( fileno(stdout) );
    }
    
    typedef AnsiConsoleColourCodes PlatformConsoleColourCodes;
    
} // namespace Catch

#endif // not Windows

namespace {
    struct NoConsoleColourCodes : IConsoleColourCodes {
        void set( Colours ) {}
    };
}

namespace Catch {

    TextColour::TextColour( Colours colour ) : m_impl( NULL ) {
        static bool s_shouldUseColour = shouldUseColourForPlatform() &&
                                        !isDebuggerActive();
        if( s_shouldUseColour )
            m_impl = new PlatformConsoleColourCodes();
        else
            m_impl = new NoConsoleColourCodes();

        if( colour )
            set( colour );
    }

    TextColour::~TextColour() {
        delete m_impl;
    }

    void TextColour::set( Colours colour ) {
        m_impl->set( colour );
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONSOLE_COLOUR_IMPL_HPP_INCLUDED
