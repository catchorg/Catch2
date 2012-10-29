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

namespace Catch {

#if defined( CATCH_CONFIG_USE_POSIX_COLOUR_CODES )

    // use POSIX console terminal codes
    // Implementation contributed by Adam Strzelecki (http://github.com/nanoant)
    // https://github.com/philsquared/Catch/pull/131
    
    TextColour::TextColour( Colours colour ) {
        if( colour )
            set( colour );
    }

    TextColour::~TextColour() {
        set( TextColour::None );
    }

    void TextColour::set( Colours colour ) {
        if( isatty( fileno(stdout) ) ) {
            switch( colour ) {
                case TextColour::FileName:
                    std::cout << "\e[1m";    // bold
                    break;
                case TextColour::ResultError:
                    std::cout << "\e[1;31m"; // bright red
                    break;
                case TextColour::ResultSuccess:
                    std::cout << "\e[1;32m"; // bright green
                    break;
                case TextColour::Error:
                    std::cout << "\e[0;31m"; // dark red
                    break;
                case TextColour::Success:
                    std::cout << "\e[0;32m"; // dark green
                    break;
                case TextColour::OriginalExpression:
                    std::cout << "\e[0;36m"; // cyan
                    break;
                case TextColour::ReconstructedExpression:
                    std::cout << "\e[0;33m"; // yellow
                    break;
                case TextColour::None:
                    std::cout << "\e[0m"; // reset
            }
        }
    }
    
#elif defined ( CATCH_PLATFORM_WINDOWS )

#include <windows.h>

    namespace {
    
        WORD mapConsoleColour( TextColour::Colours colour ) {
            switch( colour ) {
                case TextColour::FileName:      
                    return FOREGROUND_INTENSITY;                    // greyed out
                case TextColour::ResultError:   
                    return FOREGROUND_RED | FOREGROUND_INTENSITY;   // bright red
                case TextColour::ResultSuccess: 
                    return FOREGROUND_GREEN | FOREGROUND_INTENSITY; // bright green
                case TextColour::Error:         
                    return FOREGROUND_RED;                          // dark red
                case TextColour::Success:       
                    return FOREGROUND_GREEN;                        // dark green      
                case TextColour::OriginalExpression:      
                    return FOREGROUND_BLUE | FOREGROUND_GREEN;      // turquoise
                case TextColour::ReconstructedExpression:    
                    return FOREGROUND_RED | FOREGROUND_GREEN;       // greeny-yellow
                default: return 0;
            }
        }
    }
    
    struct ConsoleColourImpl {
    
        ConsoleColourImpl()
        :   hStdout( GetStdHandle(STD_OUTPUT_HANDLE) ),
            wOldColorAttrs( 0 )
        {
            GetConsoleScreenBufferInfo( hStdout, &csbiInfo );
            wOldColorAttrs = csbiInfo.wAttributes;
        }
        
        ~ConsoleColourImpl() {
            SetConsoleTextAttribute( hStdout, wOldColorAttrs );
        }
        
        void set( TextColour::Colours colour ) {
            WORD consoleColour = mapConsoleColour( colour );
            if( consoleColour > 0 )
                SetConsoleTextAttribute( hStdout, consoleColour );
        }
        
        HANDLE hStdout;
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
        WORD wOldColorAttrs;
    };
    
    TextColour::TextColour( Colours colour ) 
    : m_impl( new ConsoleColourImpl() ) 
    {
        if( colour )
            m_impl->set( colour );
    }

    TextColour::~TextColour() {
        delete m_impl;
    }

    void TextColour::set( Colours colour ) {
        m_impl->set( colour );
    }

#else

    TextColour::TextColour( Colours ){}
    TextColour::~TextColour(){}
    void TextColour::set( Colours ){}

#endif

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONSOLE_COLOUR_IMPL_HPP_INCLUDED
