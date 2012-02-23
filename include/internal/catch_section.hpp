/*
 *  catch_section.hpp
 *  Catch
 *
 *  Created by Phil on 03/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED

#include "catch_capture.hpp"
#include "catch_totals.hpp"

#include <string>

namespace Catch
{
    class Section
    {
    public:
        ///////////////////////////////////////////////////////////////////////        
        Section
        (
            const std::string& name, 
            const std::string& description,
            const std::string& filename,
            std::size_t line
        )
        :   m_name( name ),
            m_sectionIncluded( Hub::getResultCapture().sectionStarted( name, description, filename, line, m_assertions ) )
        {
        }

        ///////////////////////////////////////////////////////////////////////        
        ~Section
        ()
        {
            if( m_sectionIncluded )
                Hub::getResultCapture().sectionEnded( m_name, m_assertions );
        }
        
        ///////////////////////////////////////////////////////////////////////        
        // This indicates whether the section should be executed or not
        operator bool
        ()
        {
            return m_sectionIncluded;
        }

    private:
        
        std::string m_name;
        Counts m_assertions;
        bool m_sectionIncluded;
    };
    
} // end namespace Catch

#define INTERNAL_CATCH_SECTION( name, desc ) \
    if( Catch::Section INTERNAL_CATCH_UNIQUE_NAME( catch_internal_Section ) = Catch::Section( name, desc, __FILE__, __LINE__ ) )

#endif // TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED
