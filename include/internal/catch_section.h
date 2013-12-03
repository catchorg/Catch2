/*
 *  Created by Phil on 03/12/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_SECTION_H_INCLUDED
#define TWOBLUECUBES_CATCH_SECTION_H_INCLUDED

#include "catch_section_info.h"
#include "catch_totals.hpp"
#include "catch_timer.h"

#include <string>

namespace Catch {

    class Section {
    public:
        Section(    SourceLineInfo const& lineInfo,
                    std::string const& name,
                    std::string const& description = "" );
        ~Section();

        // This indicates whether the section should be executed or not
        operator bool();

    private:

        SectionInfo m_info;

        std::string m_name;
        Counts m_assertions;
        bool m_sectionIncluded;
        Timer m_timer;
    };

} // end namespace Catch

#ifdef CATCH_CONFIG_VARIADIC_MACROS
    #define INTERNAL_CATCH_SECTION( ... ) \
        if( Catch::Section INTERNAL_CATCH_UNIQUE_NAME( catch_internal_Section ) = Catch::Section( CATCH_INTERNAL_LINEINFO, __VA_ARGS__ ) )
#else
    #define INTERNAL_CATCH_SECTION( name, desc ) \
        if( Catch::Section INTERNAL_CATCH_UNIQUE_NAME( catch_internal_Section ) = Catch::Section( CATCH_INTERNAL_LINEINFO, name, desc ) )
#endif

#endif // TWOBLUECUBES_CATCH_SECTION_H_INCLUDED
