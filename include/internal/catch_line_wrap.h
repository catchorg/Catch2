/*
 *  Created by Phil on 11/1/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_LINE_WRAP_H_INCLUDED
#define TWOBLUECUBES_CATCH_LINE_WRAP_H_INCLUDED

#include <string>

namespace Catch {
    
    void wrapLongStrings( std::ostream& stream, const std::string& str, std::size_t columns, std::size_t indent = 0 );
    std::string wrapLongStrings( const std::string& str, std::size_t columns, std::size_t indent = 0 );
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LINE_WRAP_H_INCLUDED
