#include <catch2/catch_tag_alias.hpp>

namespace Catch {
    TagAlias::TagAlias(std::string const & _tag, SourceLineInfo _lineInfo): tag(_tag), lineInfo(_lineInfo) {}    
}
