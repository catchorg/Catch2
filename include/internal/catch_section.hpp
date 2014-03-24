/*
 *  Created by Phil on 03/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED

#include "catch_section.h"
#include "catch_capture.hpp"
#include "catch_compiler_capabilities.h"
#include "catch_timer.h"

namespace Catch {

    INTERNAL_CATCH_INLINE Section::Section(   SourceLineInfo const& lineInfo,
                        std::string const& name,
                        std::string const& description )
    :   m_info( name, description, lineInfo ),
        m_sectionIncluded( getCurrentContext().getResultCapture().sectionStarted( m_info, m_assertions ) )
    {
        m_timer.start();
    }

    INTERNAL_CATCH_INLINE Section::~Section() {
        if( m_sectionIncluded )
            getCurrentContext().getResultCapture().sectionEnded( m_info, m_assertions, m_timer.getElapsedSeconds() );
    }

    // This indicates whether the section should be executed or not
    INTERNAL_CATCH_INLINE Section::operator bool() {
        return m_sectionIncluded;
    }


} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED
