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

namespace Catch { namespace Detail {
    struct IColourImpl {
        virtual ~IColourImpl() {}
        virtual void use( Colour::Code _colourCode ) = 0;
    };
}}

#if defined ( CATCH_PLATFORM_WINDOWS ) /////////////////////////////////////////

#include <windows.h>

namespace Catch {
namespace {
    
    class Win32ColourImpl : public Detail::IColourImpl {
    public:
        Win32ColourImpl() : stdoutHandle( GetStdHandle(STD_OUTPUT_HANDLE) )
        {
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
            GetConsoleScreenBufferInfo( stdoutHandle, &csbiInfo );
            originalAttributes = csbiInfo.wAttributes;
        }
        ~Win32ColourImpl() {
            use( Colour::None );
        }
        
        virtual void use( Colour::Code _colourCode ) {
            switch( _colourCode ) {
                case Colour::None:      return setTextAttribute( originalAttributes );
                case Colour::White:     return setTextAttribute( FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE );
                case Colour::Red:       return setTextAttribute( FOREGROUND_RED );
                case Colour::Green:     return setTextAttribute( FOREGROUND_GREEN );
                case Colour::Blue:      return setTextAttribute( FOREGROUND_BLUE );
                case Colour::Cyan:      return setTextAttribute( FOREGROUND_BLUE | FOREGROUND_GREEN );
                case Colour::Yellow:    return setTextAttribute( FOREGROUND_RED | FOREGROUND_GREEN );
                case Colour::Grey:      return setTextAttribute( 0 );

                case Colour::LightGrey:     return setTextAttribute( FOREGROUND_INTENSITY );
                case Colour::BrightRed:     return setTextAttribute( FOREGROUND_INTENSITY | FOREGROUND_RED );
                case Colour::BrightGreen:   return setTextAttribute( FOREGROUND_INTENSITY | FOREGROUND_GREEN );
                case Colour::BrightWhite:   return setTextAttribute( FOREGROUND_INTENSITY |  FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE );

                case Colour::Bright: throw std::logic_error( "not a colour" );
            }
        }

    private:    
        void setTextAttribute( WORD _textAttribute ) {
            SetConsoleTextAttribute( stdoutHandle, _textAttribute );
        }
        HANDLE stdoutHandle;
        WORD originalAttributes;
    };

    WORD mapConsoleColour( IConsoleColourCodes::Colours colour ) {
        enum Win32Colours {
                Grey = FOREGROUND_INTENSITY,
                BrightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
                BrightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
                BrightWhite = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
                DarkGreen = FOREGROUND_GREEN,
                Cyan = FOREGROUND_BLUE | FOREGROUND_GREEN,
                Yellow = FOREGROUND_RED | FOREGROUND_GREEN
        };
        switch( colour ) {
            case IConsoleColourCodes::FileName:             return Grey;
            case IConsoleColourCodes::ResultError:          return BrightRed;
            case IConsoleColourCodes::ResultSuccess:        return BrightGreen;
            case IConsoleColourCodes::Error:                return BrightRed;
            case IConsoleColourCodes::Success:              return DarkGreen;
            case IConsoleColourCodes::OriginalExpression:   return Cyan;
            case IConsoleColourCodes::ReconstructedExpression: return Yellow;
            case IConsoleColourCodes::SecondaryText:        return Grey;
            case IConsoleColourCodes::Headers:              return 0;
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
    Win32ColourImpl platformColourImpl;

} // end anon namespace
} // end namespace Catch

#else // Not Windows - assumed to be POSIX compatible //////////////////////////

#include <unistd.h>

namespace Catch {
namespace {

    class PosixColourImpl : public Detail::IColourImpl {
    public:
        PosixColourImpl() {
            use( Colour::None );
        }

        virtual void use( Colour::Code _colourCode ) {
            switch( _colourCode ) {
                case Colour::None:
                case Colour::White:     return setColour( "[0m" );
                case Colour::Red:       return setColour( "[0;31m" );
                case Colour::Green:     return setColour( "[0;32m" );
                case Colour::Blue:      return setColour( "[0:34m" );
                case Colour::Cyan:      return setColour( "[0;36m" );
                case Colour::Yellow:    return setColour( "[0;33m" );
                case Colour::Grey:      return setColour( "[1;30m" );

                case Colour::LightGrey:     return setColour( "[0;37m" );
                case Colour::BrightRed:     return setColour( "[1;31m" );
                case Colour::BrightGreen:   return setColour( "[1;33m" );
                case Colour::BrightWhite:   return setColour( "[1;37m" );
                
                case Colour::Bright: throw std::logic_error( "not a colour" );
            }
        }
    private:    
        void setColour( const char* _escapeCode ) {
            std::cout << '\033' << _escapeCode;
        }
    };

    // use POSIX/ ANSI console terminal codes
    // Implementation contributed by Adam Strzelecki (http://github.com/nanoant)
    // https://github.com/philsquared/Catch/pull/131

    const char* WhiteOrNormal = "[0m";
    const char* BrightRed =       "[1;31m";
    const char* BrightGreen =     "[1;32m";
//    const char* BrightWhite =     "[1;37m";
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
                case ResultError:           return BrightRed;
                case ResultSuccess:         return BrightGreen;
                case Error:                 return BrightRed;
                case Success:               return Green;
                case OriginalExpression:    return Cyan;
                case ReconstructedExpression: return Yellow;
                case SecondaryText:         return LightGrey;
                case Headers:               return WhiteOrNormal;
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
    PosixColourImpl platformColourImpl;
    
} // end anon namespace
} // end namespace Catch

#endif // not Windows

namespace {
    struct NoConsoleColourCodes : Catch::IConsoleColourCodes {
        void set( Colours ) {}
    };
}

namespace Catch {

    namespace {
        struct NoColourImpl : Detail::IColourImpl {
            void use( Colour::Code ) {}
        };
        NoColourImpl noColourImpl;        
        static const bool shouldUseColour = shouldUseColourForPlatform() &&
                                            !isDebuggerActive();
    }

    Colour::Colour( Code _colourCode ){ use( _colourCode ); }
    Colour::~Colour(){ use( None ); }
    void Colour::use( Code _colourCode ) {
        impl->use( _colourCode );
    }

    Detail::IColourImpl* Colour::impl = shouldUseColour
            ? static_cast<Detail::IColourImpl*>( &platformColourImpl )
            : static_cast<Detail::IColourImpl*>( &noColourImpl );

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
