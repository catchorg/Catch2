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
#include <vector>

namespace Catch {
    
    class LineWrapper {
    public:
        LineWrapper( std::size_t _indent, std::size_t _right );
        LineWrapper( std::size_t _right );
        LineWrapper();

        LineWrapper& setIndent( std::size_t _indent );
        LineWrapper& setRight( std::size_t _right );

        LineWrapper& wrap( std::string const& _str );
        
        std::ostream& intoStream( std::ostream& stream ) const;
        std::string toString() const;

        typedef std::vector<std::string>::const_iterator const_iterator;

        const_iterator begin() const { return lines.begin(); }
        const_iterator end() const { return lines.end(); }
        
    private:
        void wrapInternal( std::string const& _str );
        void addLine( const std::string& _line );

        std::string indent;
        std::size_t right;
        std::size_t nextTab;
        std::size_t tab;
        std::vector<std::string> lines;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LINE_WRAP_H_INCLUDED
