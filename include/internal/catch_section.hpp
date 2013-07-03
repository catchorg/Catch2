/*
 *  Created by Phil on 03/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED

#include "catch_capture.hpp"
#include "catch_totals.hpp"
#include "catch_compiler_capabilities.h"

#include <string>

namespace Catch {

    class Section {
    public:
        Section(    SourceLineInfo const& lineInfo,
                    std::string const& name,
                    std::string const& description = "" )
        :   m_info( name, description, lineInfo ),
            m_sectionIncluded( getCurrentContext().getResultCapture().sectionStarted( m_info, m_assertions ) )
        {}

        ~Section() {
            if( m_sectionIncluded )
                getCurrentContext().getResultCapture().sectionEnded( m_info, m_assertions );
        }

        // This indicates whether the section should be executed or not
        operator bool() {
            return m_sectionIncluded;
        }

    private:
        SectionInfo m_info;

        std::string m_name;
        Counts m_assertions;
        bool m_sectionIncluded;
    };

} // end namespace Catch

#ifdef CATCH_CONFIG_VARIADIC_MACROS
    #define INTERNAL_CATCH_SECTION( ... ) \
        if( Catch::Section INTERNAL_CATCH_UNIQUE_NAME( catch_internal_Section ) = Catch::Section( CATCH_INTERNAL_LINEINFO, __VA_ARGS__ ) )
#else
    #define INTERNAL_CATCH_SECTION( name, desc ) \
        if( Catch::Section INTERNAL_CATCH_UNIQUE_NAME( catch_internal_Section ) = Catch::Section( CATCH_INTERNAL_LINEINFO, name, desc ) )
#endif

#endif // TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED
