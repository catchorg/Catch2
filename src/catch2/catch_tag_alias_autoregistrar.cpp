#include <catch2/catch_tag_alias_autoregistrar.h>
#include <catch2/catch_compiler_capabilities.h>
#include <catch2/catch_interfaces_registry_hub.h>

namespace Catch {

    RegistrarForTagAliases::RegistrarForTagAliases(char const* alias, char const* tag, SourceLineInfo const& lineInfo) {
        CATCH_TRY {
            getMutableRegistryHub().registerTagAlias(alias, tag, lineInfo);
        } CATCH_CATCH_ALL {
            // Do not throw when constructing global objects, instead register the exception to be processed later
            getMutableRegistryHub().registerStartupException();
        }
    }

}
