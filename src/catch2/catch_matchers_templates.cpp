#include <catch2/catch_matchers_templates.hpp>

namespace Catch {
namespace Matchers {
    namespace Impl {
        std::vector<const void*> vector_cat(std::vector<void const*> && lhs, std::vector<void const*> && rhs) {
            lhs.insert(lhs.end(), rhs.begin(), rhs.end());
            return std::move(lhs);
        }

        std::vector<const void*> vector_cat(std::vector<void const*> && lhs, void const* rhs) {
            lhs.push_back(rhs);
            return std::move(lhs);
        }

        std::vector<const void*> vector_cat(void const* lhs, std::vector<void const*> && rhs) {
            rhs.insert(rhs.begin(), lhs);
            return std::move(rhs);
        }
        std::string describe_multi_matcher(StringRef combine, std::string const* descriptions_begin, std::string const* descriptions_end) {
            std::string description;
            std::size_t combined_size = 4;
            for ( auto desc = descriptions_begin; desc != descriptions_end; ++desc ) {
                combined_size += desc->size();
            }
            combined_size += (descriptions_end - descriptions_begin - 1) * combine.size();

            description.reserve(combined_size);

            description += "( ";
            bool first = true;
            for( auto desc = descriptions_begin; desc != descriptions_end; ++desc ) {
                if( first )
                    first = false;
                else
                    description += combine;
                description += *desc;
            }
            description += " )";
            return description;
        }
    }
} // namespace Matchers
} // namespace Catch
