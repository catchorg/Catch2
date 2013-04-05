/*
 *  Created by Phil on 25/2/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONSOLE_COLOUR_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONSOLE_COLOUR_HPP_INCLUDED

#include "catch_common.h"

namespace Catch {

    namespace Detail {
        struct IColourImpl;
    }

    struct Colour {
        enum Code {
            None = 0,
            
            White,
            Red,
            Green,
            Blue,
            Cyan,
            Yellow,
            Grey,
            
            Bright = 0x10,
            
            BrightRed = Bright | Red,
            BrightGreen = Bright | Green,
            LightGrey = Bright | Grey,
            BrightWhite = Bright | White,
            
            // By intention
            FileName = Grey,
            ResultError = BrightRed,
            ResultSuccess = BrightGreen,
            
            Error = BrightRed,
            Success = Green,
            
            OriginalExpression = Cyan,
            ReconstructedExpression = Yellow,
            
            SecondaryText = Grey,
            Headers = White
        };

        Colour( Code _colourCode );
        ~Colour();
        static void use( Code _colourCode );
        
    private:
        static Detail::IColourImpl* impl;
    };

    struct IConsoleColourCodes : NonCopyable {
        enum Colours {
            None,
            
            FileName,
            ResultError,
            ResultSuccess,
            
            Error,
            Success,
            
            OriginalExpression,
            ReconstructedExpression,
            
            SecondaryText,
            Headers
        };

        virtual void set( Colours colour ) = 0;
    };

    class TextColour : public IConsoleColourCodes {
    public:
        TextColour( Colours colour = None );
        void set( Colours colour );
        ~TextColour();
        
    private:
        IConsoleColourCodes* m_impl;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONSOLE_COLOUR_HPP_INCLUDED
