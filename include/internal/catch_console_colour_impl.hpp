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
        switch( colour ) {
            case IConsoleColourCodes::FileName:
                return FOREGROUND_INTENSITY;                    // greyed out
            case IConsoleColourCodes::ResultError:   
                return FOREGROUND_RED | FOREGROUND_INTENSITY;   // bright red
            case IConsoleColourCodes::ResultSuccess:
                return FOREGROUND_GREEN | FOREGROUND_INTENSITY; // bright green
            case IConsoleColourCodes::Error:
                return FOREGROUND_RED | FOREGROUND_INTENSITY;   // bright red
            case IConsoleColourCodes::Success:
                return FOREGROUND_GREEN;                        // dark green      
            case IConsoleColourCodes::OriginalExpression:
                return FOREGROUND_BLUE | FOREGROUND_GREEN;      // turquoise
            case IConsoleColourCodes::ReconstructedExpression:    
                return FOREGROUND_RED | FOREGROUND_GREEN;       // greeny-yellow
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
        
    struct AnsiConsoleColourCodes : IConsoleColourCodes {
    
        ~AnsiConsoleColourCodes() {
            set( None );
        }

        void set( Colours colour ) {
            const char colourEscape = '\033';
            switch( colour ) {
                case FileName:
                    std::cout << colourEscape << "[0m";    // white/ normal
                    break;
                case ResultError:
                    std::cout << colourEscape << "[0;31m"; // red
                    break;
                case ResultSuccess:
                    std::cout << colourEscape << "[0;32m"; // green
                    break;
                case Error:
                    std::cout << colourEscape << "[1;31m"; // bold red
                    break;
                case Success:
                    std::cout << colourEscape << "[1;32m"; // bold green
                    break;
                case OriginalExpression:
                    std::cout << colourEscape << "[0;36m"; // cyan
                    break;
                case ReconstructedExpression:
                    std::cout << colourEscape << "[0;33m"; // yellow
                    break;
                case None:
                    std::cout << colourEscape << "[0m"; // reset
            }
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
