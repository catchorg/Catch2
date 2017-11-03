#include "catch_tag_alias_autoregistrar.h"
#include "catch_interfaces_registry_hub.h"

namespace Catch {
    
    RegistrarForTagAliases::RegistrarForTagAliases(char const* alias, char const* tag, SourceLineInfo const& lineInfo) {
        try {
            getMutableRegistryHub().registerTagAlias(alias, tag, lineInfo);
        } catch (...) {
            // Do not throw when constructing global objects, instead register the exception to be processed later
            getMutableRegistryHub().registerStartupException();
        }
    }

}
