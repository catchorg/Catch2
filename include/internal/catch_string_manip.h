/*
 *  Created by Martin on 25/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_STRING_MANIP_H_INCLUDED
#define TWOBLUECUBES_CATCH_STRING_MANIP_H_INCLUDED

#include <string>
#include <iosfwd>

namespace Catch {

    bool startsWith( std::string const& s, std::string const& prefix );
    bool startsWith( std::string const& s, char prefix );
    bool endsWith( std::string const& s, std::string const& suffix );
    bool endsWith( std::string const& s, char suffix );
    bool contains( std::string const& s, std::string const& infix );
    void toLowerInPlace( std::string& s );
    std::string toLower( std::string const& s );
    std::string trim( std::string const& str );
    bool replaceInPlace( std::string& str, std::string const& replaceThis, std::string const& withThis );

    struct pluralise {
        pluralise( std::size_t count, std::string const& label );

        friend std::ostream& operator << ( std::ostream& os, pluralise const& pluraliser );

        std::size_t m_count;
        std::string m_label;
    };
}

#endif // TWOBLUECUBES_CATCH_STRING_MANIP_H_INCLUDED

