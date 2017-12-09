/*
 *  Created by Phil on 27/11/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_common.h"
#include "catch_context.h"
#include "catch_interfaces_config.h"

#include <cstring>
#include <ostream>

namespace Catch {

    bool SourceLineInfo::empty() const noexcept {
        return file[0] == '\0';
    }
    bool SourceLineInfo::operator == ( SourceLineInfo const& other ) const noexcept {
        return line == other.line && (file == other.file || std::strcmp(file, other.file) == 0);
    }
    bool SourceLineInfo::operator < ( SourceLineInfo const& other ) const noexcept {
        return line < other.line || ( line == other.line && (std::strcmp(file, other.file) < 0));
    }

    std::ostream& operator << ( std::ostream& os, SourceLineInfo const& info ) {
#ifndef __GNUG__
        os << info.file << '(' << info.line << ')';
#else
        os << info.file << ':' << info.line;
#endif
        return os;
    }

    std::string StreamEndStop::operator+() const {
        return std::string();
    }

    NonCopyable::NonCopyable() = default;
    NonCopyable::~NonCopyable() = default;

}
