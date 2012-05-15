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

    struct ConsoleColourImpl;
    
    class TextColour : NonCopyable {
    public:
        
        enum Colours {
            None,
            
            FileName,
            ResultError,
            ResultSuccess,
            
            Error,
            Success,
            
            OriginalExpression,
            ReconstructedExpression
        };
        
        TextColour( Colours colour = None );
        void set( Colours colour );
        ~TextColour();
        
    private:
        ConsoleColourImpl* m_impl;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONSOLE_COLOUR_HPP_INCLUDED
