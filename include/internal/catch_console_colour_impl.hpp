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

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef __AFXDLL
#include <AfxWin.h>
#else
#include <windows.h>
#endif

namespace Catch {
namespace {

    class Win32ColourImpl {
    public:
        Win32ColourImpl() : stdoutHandle( GetStdHandle(STD_OUTPUT_HANDLE) )
        {
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
            GetConsoleScreenBufferInfo( stdoutHandle, &csbiInfo );
            originalAttributes = csbiInfo.wAttributes;
        }

        void use( Colour::Code _colourCode ) {
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
                case Colour::BrightWhite:   return setTextAttribute( FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE );

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

    inline bool shouldUseColourForPlatform() {
        return true;
    }

    typedef Win32ColourImpl PlatformColourImpl;

} // end anon namespace
} // end namespace Catch

#else // Not Windows - assumed to be POSIX compatible //////////////////////////

#include <unistd.h>

namespace Catch {
namespace {

    // use POSIX/ ANSI console terminal codes
    // Thanks to Adam Strzelecki for original contribution
    // (http://github.com/nanoant)
    // https://github.com/philsquared/Catch/pull/131
    class PosixColourImpl {
    public:
        void use( Colour::Code _colourCode ) {
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
                case Colour::BrightGreen:   return setColour( "[1;32m" );
                case Colour::BrightWhite:   return setColour( "[1;37m" );

                case Colour::Bright: throw std::logic_error( "not a colour" );
            }
        }
    private:
        void setColour( const char* _escapeCode ) {
            std::cout << '\033' << _escapeCode;
        }
    };

    inline bool shouldUseColourForPlatform() {
        return isatty(STDOUT_FILENO);
    }

    typedef PosixColourImpl PlatformColourImpl;

} // end anon namespace
} // end namespace Catch

#endif // not Windows

namespace Catch {

    template <typename Impl>
    struct ColourChange
    {
        static Impl impl;
        static const bool shouldUseColour;
    };
    template <typename Impl>
    Impl ColourChange<Impl>::impl;
    template <typename Impl>
    const bool ColourChange<Impl>::shouldUseColour = shouldUseColourForPlatform() &&
                                            !isDebuggerActive();;

    INTERNAL_CATCH_INLINE Colour::Colour( Code _colourCode ) {
        if( ColourChange<PlatformColourImpl>::shouldUseColour ) ColourChange<PlatformColourImpl>::impl.use( _colourCode );
    }
    INTERNAL_CATCH_INLINE Colour::~Colour() {
        if( ColourChange<PlatformColourImpl>::shouldUseColour ) ColourChange<PlatformColourImpl>::impl.use( Colour::None );
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONSOLE_COLOUR_IMPL_HPP_INCLUDED
