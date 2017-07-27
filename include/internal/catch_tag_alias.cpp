#include "catch_tag_alias.h"

namespace Catch {
    TagAlias::TagAlias(std::string const & _tag, SourceLineInfo _lineInfo): tag(_tag), lineInfo(_lineInfo) {}    
}
