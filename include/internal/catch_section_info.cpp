/*
 *  Created by Martin on 01/08/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_section_info.h"

namespace Catch {

    SectionInfo::SectionInfo
        (   SourceLineInfo const& _lineInfo,
            std::string const& _name,
            std::string const& _description )
    :   name( _name ),
        description( _description ),
        lineInfo( _lineInfo )
    {}

    SectionEndInfo::SectionEndInfo( SectionInfo const& _sectionInfo, Counts const& _prevAssertions, double _durationInSeconds )
    : sectionInfo( _sectionInfo ), prevAssertions( _prevAssertions ), durationInSeconds( _durationInSeconds )
    {}

} // end namespace Catch
