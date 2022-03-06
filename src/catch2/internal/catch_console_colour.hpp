
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_CONSOLE_COLOUR_HPP_INCLUDED
#define CATCH_CONSOLE_COLOUR_HPP_INCLUDED

#include <catch2/internal/catch_unique_ptr.hpp>

#include <iosfwd>

namespace Catch {

    struct IConfig;
    class IStream;

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
            BrightYellow = Bright | Yellow,

            // By intention
            FileName = LightGrey,
            Warning = BrightYellow,
            ResultError = BrightRed,
            ResultSuccess = BrightGreen,
            ResultExpectedFailure = Warning,

            Error = BrightRed,
            Success = Green,

            OriginalExpression = Cyan,
            ReconstructedExpression = BrightYellow,

            SecondaryText = LightGrey,
            Headers = White
        };
    };

    class ColourImpl {
    protected:
        //! The associated stream of this ColourImpl instance
        IStream const* m_stream;
    public:
        ColourImpl( IStream const* stream ): m_stream( stream ) {}

        class ColourGuard {
            ColourImpl const* m_colourImpl;
            bool m_moved = false;
            friend std::ostream& operator<<(std::ostream& lhs,
                                            ColourGuard const&) {
                return lhs;
            }
        public:
            ColourGuard( Colour::Code code,
                         ColourImpl const* colour );
            ColourGuard( ColourGuard const& rhs ) = delete;
            ColourGuard& operator=( ColourGuard const& rhs ) = delete;
            ColourGuard( ColourGuard&& rhs );
            ColourGuard& operator=( ColourGuard&& rhs );
            ~ColourGuard();
        };

        virtual ~ColourImpl(); // = default
        ColourGuard startColour( Colour::Code colourCode );

    private:
        virtual void use( Colour::Code colourCode ) const = 0;
    };

    //! Provides ColourImpl based on global config and target compilation platform
    Detail::unique_ptr<ColourImpl> makeColourImpl( IConfig const* config, IStream const* stream );

} // end namespace Catch

#endif // CATCH_CONSOLE_COLOUR_HPP_INCLUDED
